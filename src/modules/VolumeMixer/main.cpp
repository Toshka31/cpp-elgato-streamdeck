#include <cstdio>
#include <pulse/pulseaudio.h>
#include <gtk/gtk.h>


#include <chrono>
#include <thread>
#include <cstring>
#include <iostream>
#include <vector>
#include <map>
#include <mutex>

void pa_sink_input_cb(pa_context *c, const pa_sink_input_info *i, int eol, void *userdata);
void pa_sink_volume_cb(pa_context *c, int success, void *userdata);

class PulseSystem
{
 public:
  PulseSystem()
  {
    pa_ml = pa_threaded_mainloop_new();
    pa_api = pa_threaded_mainloop_get_api(pa_ml);

    pa_Context = pa_context_new(pa_api, "deepin");
    pa_context_connect(pa_Context, nullptr, static_cast<pa_context_flags_t>(0), nullptr);
    pa_context_set_state_callback(pa_Context, pa_state_cb, this);

    pa_threaded_mainloop_start(pa_ml);
  }

  ~PulseSystem()
  {
    pa_context_disconnect(pa_Context);
    pa_context_unref(pa_Context);
    pa_threaded_mainloop_free(pa_ml);
  }

  void tick()
  {
    updateSinkInputs();
  }

  void volumeDown()
  {
    for (const auto &dev : m_sinkinputs) {
      auto volume = dev.second.volume;
      for (auto c = 0; c < volume.channels; ++c)
        volume.values[c] -= (PA_VOLUME_NORM / 100) * 5;
      pa_context_set_sink_input_volume(pa_Context, dev.second.index, &volume, nullptr, nullptr);
    }
  }

  void print()
  {
    std::cout << "Devices:" << std::endl;

    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto &dev : m_sinkinputs)
    {
      std::cout << dev.second.name << ":" << std::endl;
      for (auto i = 0; i < dev.second.volume.channels; ++i)
        std::cout << i << " = "<< (dev.second.volume.values[i] / (double)PA_VOLUME_NORM) * 100 << std::endl;
    }
  }
 private:
  static void pa_state_cb(pa_context *c, void *userdata)
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

  static void pa_sink_input_cb(pa_context *c, const pa_sink_input_info *i, int eol, void *userdata)
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
 public:
  struct SinkInput {
    uint32_t index{};
    pa_cvolume volume{};
    std::string icon;
    std::string name;
  };

  void addSinkInput(const SinkInput& si)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_sinkinputs_new.find(si.index);
    if (it != m_sinkinputs_new.end())
      it->second = si;
    else
      m_sinkinputs_new[si.index] = si;
  }

  void updateSinkInputs()
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

  std::vector<unsigned char> getIcon(const std::string &icon)
  {
    gtk_init(nullptr, nullptr);
    GdkPixbuf *pixbuf = gtk_icon_theme_load_icon (gtk_icon_theme_get_default (), icon.c_str(), 128, GTK_ICON_LOOKUP_NO_SVG, nullptr);
    gchar * jpg_data;
    gsize jpg_data_len;
    gdk_pixbuf_save_to_buffer(pixbuf, &jpg_data, &jpg_data_len, "jpeg", nullptr, nullptr);

    std::vector<unsigned char> ret(jpg_data, jpg_data + jpg_data_len);
    return ret;
  }

 private:
  pa_threaded_mainloop *pa_ml{nullptr};
  pa_mainloop_api *pa_api{nullptr};
  pa_context *pa_Context{nullptr};

  pa_operation *pa_sinkinputs_update{nullptr};

  std::mutex m_mutex;

  std::map<uint32_t, SinkInput> m_sinkinputs_new;
  std::map<uint32_t, SinkInput> m_sinkinputs;
};

using namespace std::chrono_literals;

int main(int argc, const char *argv[])
{
  PulseSystem paapi;

  int iter = 0;
  for (;;)
  {
    ++iter;
    std::cout << iter << std::endl;
    paapi.tick();
    if  (iter % 10 == 0)
      paapi.volumeDown();
    paapi.print();
    std::this_thread::sleep_for(100ms);
  }

  return 0;
}