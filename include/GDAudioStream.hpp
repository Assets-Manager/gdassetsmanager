#ifndef GDAUDIOSTREAM_HPP
#define GDAUDIOSTREAM_HPP

#include "godot_cpp/classes/ref.hpp"
#include <cstdint>
#include <godot_cpp/classes/audio_stream_playback_resampled.hpp>
#include <AudioLoaders/IAudioLoader.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/audio_stream.hpp>

using namespace godot;

class GDAudioStreamPlayback;
class GDAudioStream : public AudioStream
{
    GDCLASS(GDAudioStream, AudioStream);
    public:
        friend GDAudioStreamPlayback;
        static void _bind_methods()
        {
            godot::ClassDB::bind_method(godot::D_METHOD("load"), &GDAudioStream::Load);
        }

        Error Load(String path);

        Ref<AudioStreamPlayback> _instantiate_playback() const override;
        String _get_stream_name() const override;
        double _get_length() const override;
        bool _is_monophonic() const override;
        Dictionary _get_tags() const override;
        bool _has_loop() const override;

        ~GDAudioStream()
        {
            if(m_Loader) delete m_Loader;
        }

    private:
        IAudioLoader *m_Loader{};
};

class GDAudioStreamPlayback : public AudioStreamPlaybackResampled
{
    GDCLASS(GDAudioStreamPlayback, AudioStreamPlaybackResampled);
    public:
        GDAudioStreamPlayback() = default;
        Ref<GDAudioStream> Stream;

        static void _bind_methods() {}

        int32_t _mix_resampled(AudioFrame *p_dst_buffer, int32_t p_frame_count) override;
        float _get_stream_sampling_rate() const override;

        void _start(double p_from_pos) override;
	    void _stop() override;
	    bool _is_playing() const override;
	    int32_t _get_loop_count() const override;
	    double _get_playback_position() const override;
	    void _seek(double p_position) override;

        ~GDAudioStreamPlayback() = default;
    private:
        bool m_Active{};
        int64_t m_Offset{};
};

#endif