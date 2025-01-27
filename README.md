
# Introduction

A K-Means clustering visualization program written on Gtk and Cairo.
The program was written in purpose of demonstating the group member's understanding in lecture of COMP 4800 at University of Windsor.

# Team member
Aryan 

Astha Singh

Ngoc Doanh Tran

# File Structure
a3.c ---------- main program

a3data.txt ----------- synthetic input data file

# Input file structure
For the program to read the input file properly, please refer to the following example file structure as **a3data.txt**


# Compiling and Run
To start the program, run the following command:
```
gcc $(pkg-config --cflags gtk4 glib-2.0 ) -o a3 a3.c $(pkg-config --libs gtk4 glib-2.0) -lm -lpthread

./a3 <file input> <framerate>
Ex: ./a3 a3data.txt 1000
```

# Dependencies
GTK	-------- https://download.gnome.org/sources/gtk/

GLib -------- https://download.gnome.org/sources/glib/

Pango -------- https://download.gnome.org/sources/pango/

Gdk-pixbuf -------- https://download.gnome.org/sources/gdk-pixbuf/

ATK	-------- https://download.gnome.org/sources/atk/

GObject-Introspection -------- https://download.gnome.org/sources/gobject-introspection/

Epoxy -------- https://download.gnome.org/sources/libepoxy/

# Installation
For Linux
| Distribution | Binary package | Development package | Additional packages |
| :---:   | :---: | :---: | :---: |
| Arch | gtk4   | -   |-   |
| Debian/Ubuntu | libgtk-4-1 | libgtk-4-dev | gtk-4-examples |
| Fedora | gtk4   | gtk4-devel | -   |

For furthermore:
https://www.gtk.org/docs/installations/linux

# References
https://www.gnu.org/software/libiconv/#TOCdownloading

https://syskall.com/kmeans.js/

https://en.wikipedia.org/wiki/K-means_clustering


