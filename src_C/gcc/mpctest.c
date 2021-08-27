#include <mpc.h>
#include <mpfr.h>
int
main ()
{

    mpfr_t n;
    mpfr_t x;
    mpc_t c;
    int t;
    mpfr_init (n);
    mpfr_init (x);
    mpfr_atan2 (n, n, x, MPFR_RNDN);
    mpfr_erfc (n, x, MPFR_RNDN);
    mpfr_subnormalize (x, t, MPFR_RNDN);
    mpfr_clear(n);
    mpfr_clear(x);
    mpc_init2 (c, 53);
    mpc_set_ui_ui (c, 1, 1, MPC_RNDNN);
    mpc_cosh (c, c, MPC_RNDNN);
    mpc_pow (c, c, c, MPC_RNDNN);
    mpc_acosh (c, c, MPC_RNDNN);
    mpc_clear (c);

  ;
  return 0;
}

