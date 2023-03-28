#pragma once

#include <cstdio>
#include <pulse/pulseaudio.h>
#include <gtk/gtk.h>

#include <string>
#include <map>
#include <vector>
#include <mutex>

class PulseSystem
{
public:
    PulseSystem();

    ~PulseSystem();

    void volumeUp(const std::string &sink_name);

    void volumeDown(const std::string &sink_name);

    std::vector<std::pair<std::string, unsigned short>> getVolumes();

    static std::vector<unsigned char> getIcon(const std::string &icon);

private:
    struct SinkInput {
        uint32_t index{};
        pa_cvolume volume{};
        std::string icon;
        std::string name;
    };

    void addSinkInput(const SinkInput& si);

    void updateSinkInputs();

    void changeVolume(const std::string &sink_name, int value);

    static void pa_state_cb(pa_context *c, void *userdata);

    static void pa_sink_input_cb(pa_context *c, const pa_sink_input_info *i, int eol, void *userdata);

private:
    pa_threaded_mainloop *pa_ml{nullptr};
    pa_mainloop_api *pa_api{nullptr};
    pa_context *pa_Context{nullptr};

    pa_operation *pa_sinkinputs_update{nullptr};

    std::mutex m_mutex;

    std::map<uint32_t, SinkInput> m_sinkinputs_new;
    std::map<uint32_t, SinkInput> m_sinkinputs;
};
