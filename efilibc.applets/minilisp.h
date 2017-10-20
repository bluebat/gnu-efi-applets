#ifndef _MINILISP_H_
#define _MINILISP_H_


typedef enum   { ATOM, CONS, SUBR } Type;
typedef struct { char *name; } Atom;
typedef struct { Type type; void *sexp; } Sexp;
typedef struct { Sexp *car; Sexp *cdr;  } Cons;

Atom *empty_atom();
Cons *empty_cons();
Sexp *empty_sexp();

Sexp *make_atom(char *str);
Sexp *make_cons(Sexp *car, Sexp *cdr);
Sexp *make_sexp(Type type, void *sexp);

char *copy_str(char *str);
Atom *copy_atom(Atom *atom);
Cons *copy_cons(Cons *cons);
Sexp *copy_sexp(Sexp *sexp);

char *atom_to_string(Atom *atom);
char *cons_to_string(Cons *cons, int racket);
char *sexp_to_string(Sexp *sexp);

Sexp *read_atom(char *str, int beg);
Sexp *read_cons(char *str, int beg);
Sexp *read_from(char *str, int beg);
Sexp *read_sexp(char *str);

Sexp *_eval_(Sexp *sexp);
Sexp *_cons_(Sexp *car, Sexp *cdr);
Sexp *_car_(Sexp *sexp);
Sexp *_cdr_(Sexp *sexp);
Sexp *_quote_(Sexp *sexp);
Sexp *_if_(Sexp *_if, Sexp *_then, Sexp *_else);
Sexp *_atom_(Sexp *sexp);
Sexp *_eq_(Sexp *d1, Sexp *d2);

Sexp *fn_call(Sexp *fn, Sexp *args);
Sexp *_macroexpand_(Sexp *fn, Sexp *args);
Sexp *eval(Sexp *sexp, Sexp *env);
Sexp *_assoc_(Sexp *key, Sexp *pair);


static Sexp *Qnil, *Qt, *Qquote, *Qunbound, *Qerror;

int NILP(Sexp *sexp);
int LAMBDAP(Sexp *sexp);


typedef Sexp *Lisp_Object;

typedef struct {
  union {
    Lisp_Object (*a0) (void);
    Lisp_Object (*a1) (Lisp_Object);
    Lisp_Object (*a2) (Lisp_Object, Lisp_Object);
    Lisp_Object (*a3) (Lisp_Object, Lisp_Object, Lisp_Object);
    Lisp_Object (*a4) (Lisp_Object, Lisp_Object, Lisp_Object, Lisp_Object);
    Lisp_Object (*a5) (Lisp_Object, Lisp_Object, Lisp_Object, Lisp_Object,
                       Lisp_Object);
    Lisp_Object (*a6) (Lisp_Object, Lisp_Object, Lisp_Object, Lisp_Object,
                       Lisp_Object, Lisp_Object);
    Lisp_Object (*a7) (Lisp_Object, Lisp_Object, Lisp_Object, Lisp_Object,
                       Lisp_Object, Lisp_Object, Lisp_Object);
    Lisp_Object (*a8) (Lisp_Object, Lisp_Object, Lisp_Object, Lisp_Object,
                       Lisp_Object, Lisp_Object, Lisp_Object, Lisp_Object);
  } function;
  short min_args, max_args;
  const char *symbol_name;
} Subr;

/* Note that the weird token-substitution semantics of ANSI C makes
   this work for MANY and UNEVALLED.  */
#define DEFUN_ARGS_MANY   (ptrdiff_t, Lisp_Object *)
#define DEFUN_ARGS_UNEVALLED  (Lisp_Object)
#define DEFUN_ARGS_0  (void)
#define DEFUN_ARGS_1  (Lisp_Object)
#define DEFUN_ARGS_2  (Lisp_Object, Lisp_Object)
#define DEFUN_ARGS_3  (Lisp_Object, Lisp_Object, Lisp_Object)
#define DEFUN_ARGS_4  (Lisp_Object, Lisp_Object, Lisp_Object, Lisp_Object)
#define DEFUN_ARGS_5  (Lisp_Object, Lisp_Object, Lisp_Object, Lisp_Object, \
                       Lisp_Object)
#define DEFUN_ARGS_6  (Lisp_Object, Lisp_Object, Lisp_Object, Lisp_Object, \
                       Lisp_Object, Lisp_Object)
#define DEFUN_ARGS_7  (Lisp_Object, Lisp_Object, Lisp_Object, Lisp_Object, \
                       Lisp_Object, Lisp_Object, Lisp_Object)
#define DEFUN_ARGS_8  (Lisp_Object, Lisp_Object, Lisp_Object, Lisp_Object, \
                       Lisp_Object, Lisp_Object, Lisp_Object, Lisp_Object)

#define DEFUN(name, minargs, maxargs)           \
  Lisp_Object F##name DEFUN_ARGS_ ## maxargs;   \
  static Subr S##name =                         \
    { { .a ## maxargs = F##name },              \
      minargs, maxargs, #name};                 \
  Lisp_Object F##name


#endif /* _MINILISP_H_ */
