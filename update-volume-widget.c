#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <sys/stat.h>

char *card = "default";
char *control = "Master";

void print_error(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    fprintf(stderr, "\n");
    va_end(va);
}

int elem_event(snd_mixer_elem_t *elem, unsigned int mask)
{
    if (mask & SND_CTL_EVENT_MASK_VALUE) {
        FILE *fp = popen("awesome-client", "w");
        fprintf(fp, "vicious.force({volumewidget})");
        pclose(fp);
    }

    return 0;
}

snd_mixer_elem_t *lookup_elem(snd_mixer_t *handle, char *name, unsigned idx)
{
    snd_mixer_elem_t *elem;
    for (elem = snd_mixer_first_elem(handle);
            elem != NULL; elem = snd_mixer_elem_next(elem)) {
        if (snd_mixer_elem_get_type(elem) == SND_MIXER_ELEM_SIMPLE
                && strcmp(snd_mixer_selem_get_name(elem), name) == 0
                && snd_mixer_selem_get_index(elem) == idx) {
            return elem;
        }
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    int err;
    snd_mixer_t *handle;

    if ((err = snd_mixer_open(&handle, 0)) < 0) {
        print_error("Mixer open error: %s", snd_strerror(err));
        exit(err);
    }
    if ((err = snd_mixer_attach(handle, card)) < 0) {
        print_error("Mixer attach card %s error: %s", card, snd_strerror(err));
        snd_mixer_close(handle);
        exit(err);
    }
    if ((err = snd_mixer_selem_register(handle, NULL, NULL)) < 0) {
        print_error("Mixer register error: %s", snd_strerror(err));
        snd_mixer_close(handle);
        exit(err);
    }
    if ((err = snd_mixer_load(handle)) < 0) {
        print_error("Mixer load error: %s", snd_strerror(err));
        snd_mixer_close(handle);
        exit(err);
    }

    snd_mixer_elem_t *elem = lookup_elem(handle, control, 0);
    if (elem == NULL) {
        print_error("Card %s has no control %s.", card, control);
        snd_mixer_close(handle);
        exit(EXIT_FAILURE);
    }

    snd_mixer_elem_set_callback(elem, elem_event);

    int res;
    while (1) {
        res = snd_mixer_wait(handle, -1);
        if (res >= 0) {
            res = snd_mixer_handle_events(handle);
            assert(res >= 0);
        }
    }

    snd_mixer_close(handle);
}
