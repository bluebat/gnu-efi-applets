/* minilisp
A mini Lisp Interpreter
From https://github.com/siteshen/minilisp by Xiaojiang Shen
GNU-EFI porting by Wei-Lun Chao <bluebat@member.fsf.org>, 2017
*/
#include <efi.h>
#include <efilib.h>

#include <efilibc/efilibc.h>
#include <efilibc/stdio.h>
#include <efilibc/stdlib.h>
#include <efilibc/ctype.h>
#include <efilibc/string.h>
#include "minilisp.h"

#define PRINT(sexp) printf(#sexp ": %s\n", sexp_to_string(sexp));

char *sub_str(char *str, int beg, int end) {
  char *new_str;
  if (beg >= end) return NULL;
  new_str = malloc(end - beg + 1);
  bzero(new_str, end - beg + 1);
  strncpy(new_str, str + beg, end - beg);
  return new_str;
}

/* empty_*: create empty struct */
Atom *empty_atom() { Atom *atom = malloc(sizeof(Atom)); bzero(atom, sizeof(Atom)); return atom; }
Cons *empty_cons() { Cons *cons = malloc(sizeof(Cons)); bzero(cons, sizeof(Cons)); return cons; }
Sexp *empty_sexp() { Sexp *sexp = malloc(sizeof(Sexp)); bzero(sexp, sizeof(Sexp)); return sexp; }


/* {{{ make atom/cons/sexp */
/* make an atom */
Sexp *make_atom(char *str) {
  Atom *atom = empty_atom();
  atom->name = copy_str(str);
  return make_sexp(ATOM, atom);
}

/* make a cons */
Sexp *make_cons(Sexp *car, Sexp *cdr) {
  Cons *cons = empty_cons();
  cons->car = copy_sexp(car);
  cons->cdr = copy_sexp(cdr);
  return make_sexp(CONS, cons);
}

/* make a sexp */
Sexp *make_sexp(Type type, void *sexp) {
  Sexp *new_sexp = empty_sexp();
  new_sexp->type = type;
  new_sexp->sexp = sexp;
  return new_sexp;
}
/* }}} */


/* {{{ copy  str/atom/cons/sexp */
/* return a copy of str */
char *copy_str(char *str) {
  char *new_str = malloc(strlen(str) + 1);
  strcpy(new_str, str);
  return new_str;
}

/* return a copy of atom */
Atom *copy_atom(Atom *atom) {
  Atom *new_atom = empty_atom();
  if (atom) memcpy(new_atom, atom, sizeof(Atom));
  return new_atom;
}

/* return a copy of cons */
Cons *copy_cons(Cons *cons) {
  Cons *new_cons = empty_cons();
  if (cons) memcpy(new_cons, cons, sizeof(Cons));
  return new_cons;
}

/* return a copy of sexp */
Sexp *copy_sexp(Sexp *sexp) {
  Sexp *new_sexp = empty_sexp();
  if (sexp) memcpy(new_sexp, sexp, sizeof(Sexp));
  return new_sexp;
}
/* }}} */


/* {{{ string  str/atom/cons/sexp */
/* to string: atom, cons, sexp */
char *atom_to_string(Atom *atom) {
  if (!atom) return "nil";
  return atom->name;
}

char *cons_to_string(Cons *cons, int racket) {
  char *car_str;
  char *cdr_str;
  int str_len;
  char *new_str;

  if (!cons) return "nil";
  car_str = sexp_to_string(cons->car);
  if (!cons->cdr) return car_str;
  if (cons->cdr->type == ATOM) {
    cdr_str = atom_to_string((Atom *)(cons->cdr->sexp));
  } else {
    cdr_str = cons_to_string((Cons *)(cons->cdr->sexp), 0);
  }
  str_len = strlen(car_str) + strlen(cdr_str) + 6;
  new_str = malloc(str_len);

  bzero(new_str, str_len);
  if (racket) strcat(new_str, "(");
  strcat(new_str, car_str);
  if (strcmp(cdr_str, "nil")) {
    if (cons->cdr->type == ATOM) {
      strcat(new_str, " . ");
    } else if (cons->cdr->type == CONS) {
      strcat(new_str, " ");
    }
    strcat(new_str, cdr_str);
  }
  if (racket) strcat(new_str, ")");
  return new_str;
}

char *sexp_to_string(Sexp *sexp) {
  if (!sexp) return "nil";
  switch (sexp->type) {
  case ATOM: return atom_to_string((Atom *)(sexp->sexp));
  case CONS: return cons_to_string((Cons *)(sexp->sexp), 1);
  default  : return "nil";                      /* gcc warning */
  }
}
/* }}} */


/* {{{ read atom/cons/sexp */
static int read_from_string_index = 0;
Sexp *read_atom(char *str, int beg) {
  int atom_len = 0;
  char *name = NULL;

  while (isspace(str[beg])) beg++;
  while (str[beg] && !isspace(str[beg]) && str[beg] != '(' && str[beg] != ')') {
    beg++;
    atom_len++;
  }
  name = sub_str(str, beg - atom_len, beg);
  read_from_string_index = beg;
  return make_atom(name);
}

Sexp *read_cons(char *str, int beg) {
  Sexp *car;
  Sexp *cdr;

  while (isspace(str[beg])) beg++;
  if (str[beg] == ')') {
    beg++;
    read_from_string_index = beg;
    return empty_sexp();
  }
  if (str[beg] == '.') return read_atom(str, beg + 1);
  car = read_from(str, beg);
  cdr = read_cons(str, read_from_string_index);
  return make_cons(car, cdr);
}

Sexp *read_from(char *str, int beg) {
  while (isspace(str[beg])) beg++;
  switch (str[beg]) {
  case '(':
    read_from_string_index++;
    return read_cons(str, beg + 1);
  case '\'':
    read_from_string_index++;
    return make_cons(Qquote, make_cons(read_from(str, beg + 1), empty_sexp()));
  case ')':
    read_from_string_index++;
    return Qnil;
  default : return read_atom(str, beg);
  }
}

Sexp *read_sexp(char *str) {
  return read_from(str, 0);
}

static Sexp *env = NULL;
Sexp *_eval_(Sexp *sexp) {
  return eval(sexp, env);
}
/* }}} */

DEFUN (cons, 2, 2) (Lisp_Object car, Lisp_Object cdr) {
  return make_cons(car, cdr);
}

DEFUN (car, 1, 1) (Lisp_Object sexp) {
  if (!sexp) return Qnil;
  switch (sexp->type) {
  case ATOM: return Qnil;
  case CONS: return ((Cons *)(sexp->sexp))->car;
  default  : return Qnil;                       /* gcc warning */
  }
}

DEFUN (cdr, 1, 1) (Lisp_Object sexp) {
  if (!sexp) return Qnil;
  switch (sexp->type) {
  case ATOM: return Qnil;
  case CONS: return ((Cons *)(sexp->sexp))->cdr;
  default  : return Qnil;                       /* gcc warning */
  }
}

DEFUN (quote, 1, 1) (Lisp_Object sexp) {
  return sexp;
}

DEFUN (_if, 2, 3) (Lisp_Object if_, Lisp_Object then_, Lisp_Object else_) {
  return !NILP(if_) ? (then_) : (else_);
}

DEFUN (eq, 2, 2) (Lisp_Object d1, Lisp_Object d2) {
  return !strcmp(sexp_to_string(d1), sexp_to_string(d2)) ? Qt : Qnil;
}

DEFUN (atom, 1, 1) (Lisp_Object sexp) {
  if (!sexp) return Qt;
  switch (sexp->type) {
  case ATOM: return Qt;
  case CONS: return Feq(sexp, Qt);
  default  : return Qnil;                       /* gcc warning */
  }
}

DEFUN (def, 2, 2) (Lisp_Object sexp1, Lisp_Object sexp2) {
  env = Fcons(Fcons(sexp1, sexp2), env);
  return sexp1;
}

Sexp *_read_(char *str) {
  return read_sexp(str);
}

Sexp *nth(int n, Lisp_Object sexp) {
  while (n > 0 && !NILP(sexp)) {
    sexp = Fcdr(sexp);
    n--;
  }
  return Fcar(sexp);
}

int NILP(Sexp *sexp) {
  return (sexp && sexp->type == ATOM && !strcmp(sexp_to_string(sexp), "nil"));
}

int LAMBDAP(Sexp *sexp) {
  char *car_str;
  if (sexp && sexp->type == CONS) {
    car_str = sexp_to_string(Fcar(sexp));
    return (!strcmp(car_str, "lambda"));
  }
  return 0;
}

int MACROP(Sexp *sexp) {
  char *car_str;
  if (sexp && sexp->type == CONS) {
    car_str = sexp_to_string(Fcar(sexp));
    return (!strcmp(car_str, "macro"));
  }
  return 0;
}

/* TODO: implement it */
Sexp *eval(Sexp *sexp, Sexp *local_env) {
  Sexp *car;
  Sexp *cdr;
  Sexp *key, *val;
  Sexp *lambda;
  Sexp *arg_list;
  Sexp *val_list;
  char *car_str;

  if (!sexp) return Qnil;

  switch (sexp->type) {
  case ATOM: return _assoc_(sexp, local_env);
  case CONS:
    car = Fcar(sexp);
    cdr = Fcdr(sexp);

    PRINT(car);
    PRINT(cdr);
    if (!NILP(Fatom(car))) {
      car_str = sexp_to_string(car);
      if (!strcmp(car_str, "quote")) {
        return Fcar(cdr);
      } else if (!strcmp(car_str, "atom")) {
        return Fatom(eval(nth(1, sexp), local_env));
      } else if (!strcmp(car_str, "eq")) {
        return Feq(eval(nth(1, sexp), local_env),
                   eval(nth(2, sexp), local_env));
      } else if (!strcmp(car_str, "car")) {
        return Fcar(eval(nth(1, sexp), local_env));
      } else if (!strcmp(car_str, "cdr")) {
        return Fcdr(eval(nth(1, sexp), local_env));
      } else if (!strcmp(car_str, "cons")) {
        return Fcons(eval(nth(1, sexp), local_env),
                     eval(nth(2, sexp), local_env));
      } else if (!strcmp(car_str, "if")) {
        return F_if(eval(nth(1, sexp), local_env),
                    eval(nth(2, sexp), local_env),
                    eval(nth(3, sexp), local_env));
      } else if (!strcmp(car_str, "def")) {
        return Fdef(nth(1, sexp),
                    eval(nth(2, sexp), local_env));
      } else if (!strcmp(car_str, "lambda")) {
        return sexp;
      } else if (!strcmp(car_str, "macro")) {
        return sexp;
      } else if (!strcmp(car_str, "macroexpand")) {
        cdr = eval(Fcar(cdr), local_env);
        return _macroexpand_(Fcar(cdr), Fcdr(cdr));
      } else if (!strcmp(car_str, "read")) {
        return _read_(sexp_to_string(Fcar(cdr)));
      } else if (!strcmp(car_str, "eval")) {
        return eval(nth(1, sexp), local_env);
      } else if (!strcmp(car_str, "unbound")) {
        return Qerror;
      } else {
        car = eval(car, local_env);
        return eval(Fcons(car, cdr), local_env);
      }
    } else {                                    /* car is a cons, eval it first */
      if (LAMBDAP(car)) {                       /* lambda */
        return fn_call(car, cdr);
      } else if (MACROP(car)) {                 /* macro */
        Sexp *expand = _macroexpand_(car, cdr);
        printf("macro expand as: %s\n", sexp_to_string(expand));
        return eval(expand, local_env);
      }
    }
  default  : return Qerror;
  }
}

/* ((lambda (x y) (+ x y)) 12 34) */
Sexp *fn_call(Sexp *fn, Sexp *args) {
  Sexp *local_env = copy_sexp(env);
  Sexp *arg_list = (nth(1, fn));
  Sexp *val_list = args;
  Sexp *body_list = Fcar(Fcdr(Fcdr(fn)));
  Sexp *car1, *cdr1, *car2, *cdr2;
  do {
    car1 = Fcar(arg_list);
    cdr1 = Fcdr(arg_list);
    car2 = Fcar(val_list);
    cdr2 = Fcdr(val_list);
    PRINT(("before fn_call", local_env));
    local_env = Fcons(Fcons(car1, _eval_(car2)), local_env);
    PRINT(("after  fn call", local_env));
  } while (!NILP(cdr1) && !NILP(cdr2));
  return eval(body_list, local_env);
}

Sexp *_macroexpand_(Sexp *macro, Sexp *args) {
  Sexp *local_env = copy_sexp(env);
  Sexp *arg_list = (nth(1, macro));
  Sexp *val_list = args;
  Sexp *body_list = Fcar(Fcdr(Fcdr(macro)));
  Sexp *car1, *cdr1, *car2, *cdr2;
  do {
    car1 = Fcar(arg_list);
    cdr1 = Fcdr(arg_list);
    car2 = Fcar(val_list);
    cdr2 = Fcdr(val_list);
    PRINT(("before macroexpand", local_env));
    local_env = Fcons(Fcons(car1, car2), local_env);
  } while (!NILP(cdr1) && !NILP(cdr2));
  PRINT(body_list);
  return eval(body_list, local_env);
}

Sexp *_assoc_(Sexp *key, Sexp *pair) {
  Sexp *car = Fcar(pair);
  Sexp *cdr = Fcdr(pair);
  if (NILP(key))  return Qnil;
  if (NILP(pair)) return Qunbound;
  if (!NILP(Feq(Fcar(car), key))) {
    return Fcdr(car);
  } else {
    return _assoc_(key, Fcdr(pair));
  }
}

/* init env with some testing sexp ... */
void init_env() {
  char *keys [] = { "os",  "editor", "who"   };
  char *vals [] = { "mac", "emacs",  "xshen" };
  Sexp *key;
  Sexp *val;
  Sexp *pair;
  int i = 0;

  Qunbound = make_atom("unbound");
  Qnil = make_atom("nil");
  Qt = make_atom("t");
  Qquote = make_atom("quote");
  Qerror = make_atom("error");

  for (i = 0; i < 3; ++i) {
    key = make_atom(keys[i]);
    val = make_atom(vals[i]);
    pair = Fcons(key, val);
    env = Fcons(pair, env);
  }
}

void repl() {
  char str[256];
  Sexp *sexp;
  Sexp *value;

  while (strcmp(str, "quit") != 0) {
    printf("env  : %s\n", sexp_to_string(env));
    printf("lisp > ");
    gets(str);
    /* printf("=== I: %s\n", str); */
    sexp = _read_(str);
    /* printf("=== O: %s\n", sexp_to_string(sexp)); */
    value = _eval_(sexp);
    printf("=== Value: %s\n", sexp_to_string(value));
  }
}

void read_print(char *str) {
  Sexp *sexp = _read_(str);
  printf("read: \"%s\" => %s\n", str, sexp_to_string(sexp));
}

EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  UINTN argc;
  CHAR16 **Argv;

  InitializeLib(ImageHandle, SystemTable);
  efilibc_init(ImageHandle);
  argc = GetShellArgcArgv(ImageHandle, &Argv);

  char **argv;
  unsigned int i, j;
  argv = malloc(argc * sizeof(char *));
  for(i=0; i<argc; i++) {
    *(argv+i) = malloc((StrLen(Argv[i])+1) * sizeof(char));
    for (j=0; j<StrLen(Argv[i]); j++) {
      argv[i][j] = (char)Argv[i][j];
    }
    argv[i][j] = '\0';
  }

  char *str[] = {
    "hello", "(hello)", "((hello))",
    "(a b)", "(a (b))", "((a) b)", "((a) (b))",
    "(a b c)", "((a) b c)", "(a (b) c)", "((a) b c)", "((a) (b) (c))",
    "((a b) c)", "(a (b c))"
  };
  i = 5;
  for (i = 0; i < 14; i++) {
    /* printf("read(\"%s\") => %s\n", str[i], sexp_to_string(_read_(str[i]))); */
  }

  init_env();
  /* read_print("who"); */
  /* read_print("'who"); */
  /* read_print("'(who)"); */
  /* read_print("(car () 'a)"); */
  /* read_print("(car nil 'a)"); */
  /* read_print("(cons () 't)"); */
  /* read_print("(lambda () 'a)"); */
  repl();
  return 0;
}
