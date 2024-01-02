# 20-20-20 Rule for screen usage

Eye health. Doc told me the 20-20-20 rule for eyes during screen usage.

So I wrote a program that helps me do it. Notifies me when I need to take a break.

## Building from Source

Dependencies: `libnotify` and `gtk3`.

Dev Build:

```sh
git clone https://github.com/resyfer/t20.git
cd t20
mkdir build
cd build
cmake ..
make
```

Prod Build:

```sh
git clone https://github.com/resyfer/t20.git
cd t20
mkdir build
cd build
cmake -DDEBUG=1 ..
make
```

(_Optional_) Install instructions after this:
```
sudo make install
```

**NOTE**: Dev build has very small time intervals, and thus does not correspond to the 20-20-20 rule.

## Tested Platforms

OS:
  - Arch Linux x86_64

Desktop Environments:
  - KDE

Feel free to check it out in your environment and report if something doesn't work.

## TODO

- Windows Support
