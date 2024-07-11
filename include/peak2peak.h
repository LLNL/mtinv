
typedef struct {
        char phase_type[16]; /* noise or signal */
        float gvlo;
        float gvhi;
        float amp;
        float time;
        float timeminamp, timemaxamp;
        float duration;  /* Dur = fabs( timeminamp - timemaxamp ) */
        float period;    /* T = 2 * Dur */
        float frequency;  /* freq = 1 / T */
} Phase;

#define SIGNAL 1
#define NOISE  0
