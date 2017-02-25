/* test/minunit.h
 * From http://www.jera.com/techinfo/jtns/jtn002.html
 * Freely available with no warranty
 * The simplest unit test framework
 */
#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)
#define _FL __FILE__ ":" STRINGIZE(__LINE__) ": "
#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                            if (message) return message; } while (0)
extern int tests_run;
