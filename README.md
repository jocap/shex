**shex** is a simple shell for POSIX written in C.
It is inspired by [execline], but it shares no code with it.
It is developed completely independently.

[execline]: https://skarnet.org/software/execline/index.html

### Programs

-   `let variable value`: define an environment variable.
-   `send [-t ter] string [...]`: print every *string*, terminated by
    *ter* (by default, the NULL character).
-   `receive [-p] [variable ...]`: from every NULL-terminated string on
    standard input, create *variable* (like `read` in POSIX
    sh).  If *-p* is provided, create positional variables (1, 2, 3 ... 10).
-   `exp argv ...`: expand all `${variables}` in *argv* and exec into it.
