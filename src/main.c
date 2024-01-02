#include <gtk/gtk.h>
#include <libnotify/notification.h>
#include <libnotify/notify.h>
// #include <pulse/error.h>
// #include <pulse/simple.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>


#define APP_NAME	"t20"
#define SOUND_PATH	"/usr/share/sounds/freedesktop/stereo/complete.oga"

#ifdef DEBUG
#	define	TWENTY_BIG_SECS		10
#	define	TWENTY_SMALL_SECS	5
#else
#	define	TWENTY_BIG_SECS		1200
#	define	TWENTY_SMALL_SECS	20
#endif


NotifyNotification *rep_start = NULL, *rep_end = NULL;

void setup(int *argc, char*** argv);
void teardown(void);

void rep_prompt_init(void);
void rep_prompt_cb(NotifyNotification *notify, char *action, gpointer user_data);

void start_timer(int secs, GSourceFunc cb, gpointer user_data);
void stop_timer(void);

gboolean notif_show(gpointer user_data);
void play_bell(void);

enum TURN {
	BIG,
	SMALL,
	NO_START
};


guint timeout_id = 0;
int turn = NO_START;


int main(int argc, char** argv)
{
	setup(&argc, &argv);

	start_timer(0, notif_show, NULL);

	gtk_main();

	teardown();

	return EXIT_SUCCESS;
}


/*
 *
 * INITIALIZE
 *
 */
void make_daemon(void)
{
	pid_t pid = fork();

	if(pid < 0) {
		perror("Failed to fork to create a daemon");
		exit(EXIT_FAILURE);
	}

	if(pid > 0) {
		exit(EXIT_SUCCESS);
	}

	umask(0); // Unmask to allow open/read/write anywhere

	// Detach
	if(setsid() < 0) {
		perror("Failed to create a new session");
		exit(EXIT_FAILURE);
	}

	// Change working directory
	if (chdir("/") < 0) {
        fprintf(stderr, "Failed to change working directory\n");
        exit(EXIT_FAILURE);
    }

	// Close standard I/O
	// close(STDIN_FILENO); //! No sound if STDIN is closed (Failed to open audio file). HINT: idfk why
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

void setup(int *argc, char*** argv)
{
	gtk_init(argc, argv);
	if(!notify_init(APP_NAME)) {
		perror("Couldn't initialize " APP_NAME);
		exit(EXIT_FAILURE);
	}

#ifndef DEBUG
	make_daemon();
#endif

	rep_prompt_init();
}

void teardown()
{
	g_object_unref(G_OBJECT(rep_start));
	g_object_unref(G_OBJECT(rep_end));
	notify_uninit();
}

void rep_prompt_cb(NotifyNotification *notify, char *action, gpointer user_data)
{
	if(!strcmp(action, "break")) {
		start_timer(TWENTY_SMALL_SECS, notif_show, NULL);
	} else if (!strcmp(action, "next")) {
		start_timer(TWENTY_BIG_SECS, notif_show, NULL);
	}
}


/*
 *
 * REP
 * PROMPT
 *
 */

void rep_prompt_init()
{
	rep_start = notify_notification_new("20-20-20 Break!", "A well deserved 20 seconds break!\a", "appointment-soon");
	notify_notification_add_action(rep_start, "break", "Start Break", NOTIFY_ACTION_CALLBACK(rep_prompt_cb), NULL, NULL);
	notify_notification_set_timeout(rep_start, NOTIFY_EXPIRES_NEVER);

	rep_end = notify_notification_new("20-20-20 Start!", "See you in 20 minutes!\a", "appointment-soon");
	notify_notification_add_action(rep_end, "next", "Next", NOTIFY_ACTION_CALLBACK(rep_prompt_cb), NULL, NULL);
	notify_notification_set_timeout(rep_end, NOTIFY_EXPIRES_NEVER);
}

/*
 *
 * TIMER
 *
 */
void start_timer(int secs, GSourceFunc cb, gpointer user_data)
{
	timeout_id =  g_timeout_add_seconds(secs, cb, user_data);
}

void stop_timer(void)
{
	if(timeout_id != 0) {
		g_source_remove(timeout_id);
		timeout_id = 0;
	}
}

/*
 *
 * NOTIFICATION
 *
 */
gboolean notif_show(gpointer user_data)
{
	stop_timer();

	play_bell();
	if(turn == SMALL) {
		notify_notification_show(rep_start, NULL);
		turn = BIG;
	} else {
		notify_notification_show(rep_end, NULL);
		turn = SMALL;
	}

	return TRUE;
}

void play_bell(void)
{

	pid_t pid = fork();
	if(pid < 0) {
		perror("Could not play audio");
	}

	if(pid > 0) {
		wait(NULL);
	} else {
		system("paplay " SOUND_PATH);
		exit(EXIT_SUCCESS);
	}

	/*
	 * FOLLOWING CODE PRODUCES WHITE NOISE
	 *
	 * Using PulseAudio
	 */

	// pa_simple *s = NULL;
	// int error = 0;

	// pa_sample_spec ss = {
	// 						.format = PA_SAMPLE_S16LE,
	// 						.rate = 44100,
	// 						.channels = 2,
	// 					};

	// s = pa_simple_new(NULL, "Simple Example", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error);
	// if(!s) {
	// 	perror("Failed to create PulseAudio Stream");
	// 	exit(EXIT_FAILURE);
	// }

	// int audio = open(SOUND_PATH, O_RDONLY);
	// if(audio == -1) {
	// 	perror("Could open find Audio file");
	// 	pa_simple_free(s);
	// 	exit(EXIT_FAILURE);
	// }

	// // Read
	// char buffer[1024] = {0};
	// size_t bytes;

	// while ((bytes = read(audio, buffer, sizeof(buffer))) > 0) {
    //     if (pa_simple_write(s, buffer, bytes, &error) < 0) {
    //         fprintf(stderr, "pa_simple_write() failed: %s\n", pa_strerror(error));
	// 		perror("Could not play file.");
    //         break;
    //     }
    // }
	// pa_simple_drain(s, &error);

	// close(audio);
	// pa_simple_free(s);
}