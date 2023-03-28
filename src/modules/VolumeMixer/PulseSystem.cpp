#include "PulseSystem.h"

static constexpr int VOLUME_DELTA = 5;

PulseSystem::PulseSystem()
{
    pa_ml = pa_threaded_mainloop_new();
    pa_api = pa_threaded_mainloop_get_api(pa_ml);

    pa_Context = pa_context_new(pa_api, "deepin");
    pa_context_connect(pa_Context, nullptr, static_cast<pa_context_flags_t>(0), nullptr);
    pa_context_set_state_callback(pa_Context, pa_state_cb, this);

    pa_threaded_mainloop_start(pa_ml);
}

PulseSystem::~PulseSystem()
{
    pa_context_disconnect(pa_Context);
    pa_context_unref(pa_Context);
    pa_threaded_mainloop_free(pa_ml);
}

void PulseSystem::volumeUp(const std::string &sink_name)
{
    changeVolume(sink_name, VOLUME_DELTA);
}

void PulseSystem::volumeDown(const std::string &sink_name)
{
    changeVolume(sink_name, -VOLUME_DELTA);
}

std::vector<std::pair<std::string, unsigned short>> PulseSystem::getVolumes()
{
    return std::vector<std::pair<std::string, unsigned short>>();
}

void PulseSystem::addSinkInput(const SinkInput& si)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_sinkinputs_new.find(si.index);
    if (it != m_sinkinputs_new.end())
        it->second = si;
    else
        m_sinkinputs_new[si.index] = si;
}

void PulseSystem::updateSinkInputs()
{
    if (pa_sinkinputs_update) {
        auto state = pa_operation_get_state(pa_sinkinputs_update);
        if (state == PA_OPERATION_DONE) {
            pa_operation_unref(pa_sinkinputs_update);
            pa_sinkinputs_update = nullptr;
            std::lock_guard<std::mutex> lock(m_mutex);
            m_sinkinputs = std::move(m_sinkinputs_new);
        }
    }

    if (pa_sinkinputs_update == nullptr) {
        pa_sinkinputs_update = pa_context_get_sink_input_info_list(pa_Context, pa_sink_input_cb, this);
    }
}

void PulseSystem::changeVolume(const std::string &sink_name, int value)
{
    for (const auto &dev : m_sinkinputs) {
        if (dev.second.name == sink_name) {
            auto volume = dev.second.volume;
            for (auto c = 0; c < volume.channels; ++c)
                volume.values[c] += (PA_VOLUME_NORM / 100) * value;
            pa_context_set_sink_input_volume(pa_Context, dev.second.index, &volume, nullptr, nullptr);
        }
    }
}

std::vector<unsigned char> PulseSystem::getIcon(const std::string &icon) // TODO move it to util
{
    gtk_init(nullptr, nullptr);
    GdkPixbuf *pixbuf = gtk_icon_theme_load_icon (gtk_icon_theme_get_default (), icon.c_str(), 128, GTK_ICON_LOOKUP_NO_SVG, nullptr);
    gchar * jpg_data;
    gsize jpg_data_len;
    gdk_pixbuf_save_to_buffer(pixbuf, &jpg_data, &jpg_data_len, "jpeg", nullptr, nullptr);

    std::vector<unsigned char> ret(jpg_data, jpg_data + jpg_data_len);
    return ret;
}

void PulseSystem::pa_state_cb(pa_context *c, void *userdata)
{
    pa_context_state_t state;
    int *pa_ready = static_cast<int *>(userdata);

    state = pa_context_get_state(c);
    switch  (state) {
        // There are just here for reference
        case PA_CONTEXT_UNCONNECTED:
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
        case PA_CONTEXT_FAILED:
        case PA_CONTEXT_TERMINATED:
        default:
            break;
        case PA_CONTEXT_READY:
            auto *ps = static_cast<PulseSystem *>(userdata);
            ps->updateSinkInputs();
            break;
    }
}

void PulseSystem::pa_sink_input_cb(pa_context *c, const pa_sink_input_info *i, int eol, void *userdata)
{
    if (eol > 0) {
        return;
    }

    SinkInput input;

    input.index = i->index;
    input.volume = i->volume;

    const char *prop_key = nullptr;
    void *prop_state = nullptr;
    while ((prop_key=pa_proplist_iterate(i->proplist, &prop_state))) {
        if (strcmp(prop_key, "application.icon_name") == 0)
            input.icon = pa_proplist_gets(i->proplist, prop_key);
        else if (strcmp(prop_key, "application.name") == 0)
            input.name = pa_proplist_gets(i->proplist, prop_key);
    }

    auto *ps = static_cast<PulseSystem *>(userdata);
    ps->addSinkInput(input);
}
