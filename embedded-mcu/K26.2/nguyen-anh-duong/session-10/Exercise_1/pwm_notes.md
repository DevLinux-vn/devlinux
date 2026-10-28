# pwm_notes.md — Session 10, Exercise_1

## 1. Linear duty, non-linear eyes

The duty cycle is linear in knob position, and the LED's light output is close
to linear in duty as well (average current scales with duty, and a PWM'd LED is
just switching on and off fast). What is *not* linear is my perception of it.

Perceived brightness is a strongly compressed function of luminance — Stevens'
power law puts it around `L^(1/3)`, and the usual display-gamma model uses
`L^(1/2.2)`. Either way the curve is very steep near zero and flattens out as
luminance grows. So a constant step in duty is a big perceived step when the
LED is dim and a tiny one when it is bright.

The practical result on the bench: almost all the visible change is crowded into
the **lower quarter to lower third** of the knob's travel. Off to roughly a third
of the way round already looks like "dark to almost full brightness", and the
remaining upper half of the travel looks almost identical — the LED is
effectively "on" and turning further changes very little. The change is
*largest per degree* at the bottom of the range, which is also where a small
amount of extra duty matters most, so the bottom end looks twitchy while the top
end looks dead.

Two smaller things I noticed at the extremes:

- Right at the bottom there is a short region where the LED is fully dark. The
  light output there is below what is visible in a lit room, so the first few
  hundred counts of duty look like "off" rather than "very dim".
- The ADC never reaches its theoretical ends (about 10–15 counts at the bottom,
  ~4090 at the top), so even at the mechanical stops the duty is not exactly 0
  or exactly 8191. That is normal for this ADC, not a wiring fault.

The fix, if linear-feeling brightness were required, would be to apply the
inverse curve to the duty (`duty = max * (raw/max)^2.2`) or use the LEDC gamma
table. Not required here — the point is only to see that linear duty does not
feel linear.

## 2. Frequency versus resolution — the same clock has to cover both

A duty resolution of *N* bits means the PWM counter has `2^N` steps per period:
the LED is on for a fraction `duty / 2^N` of the cycle. To produce *f* Hz with
that many steps, the counter must be clocked at

```
counter_clock = f * 2^N
```

and the LEDC derives that from its source clock (APB, 80 MHz on the ESP32-S3)
with an integer-plus-fraction divider that cannot go below 1. So frequency and
resolution trade against each other: the product is what is limited, not either
one on its own.

At my chosen pair:

```
5000 Hz * 2^13 = 5000 * 8192 = 40.96 MHz   <= 80 MHz  -> divider ~ 1.95, OK
```

5 kHz is far above the ~100 Hz where flicker becomes visible (and above the
camera/eye beat range), and 8192 steps give 0.012 % duty granularity, which is
plenty for a smooth-looking fade. 13 bits is in fact the largest resolution that
fits at 5 kHz from an 80 MHz clock — 14 bits would need 81.92 MHz, just over the
limit.

Asking for 13 bits at 5 MHz requires a counter clock of 40.96 **GHz**, so the
divider would have to be about 0.002. The peripheral cannot do that, and
`ledc_timer_config()` rejects the combination instead of silently producing the
wrong frequency: it logs

```
E (...) ledc: requested frequency 5000000 and duty resolution 13 can not be
             achieved, try reducing freq_hz or duty_resolution. div_param=0
```

and returns `ESP_FAIL` — wrapped in `ESP_ERROR_CHECK` that aborts and reboots, so
the mistake shows up immediately rather than as an LED that "looks a bit odd".

The usable rule is therefore `resolution <= log2(source_clock / f)`. At 5 kHz
from 80 MHz that is `log2(16000) = 13.9`, i.e. 13 bits. Want finer resolution?
Lower the frequency. Want a higher frequency? Give up resolution.
