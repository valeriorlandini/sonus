/// @file       sonus.bufsnap.cpp
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2026 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <array>
#include <vector>

using namespace c74::min;
using Snapshot = std::vector<std::vector<double>>;

class bufsnap : public object<bufsnap>
{
public:
    MIN_DESCRIPTION	{ "Save various versions of a buffer~." };
    MIN_TAGS		{ "audio, sampling" };
    MIN_AUTHOR		{ "Valerio Orlandini" };
    MIN_RELATED		{ "buffer~, sonus.wavesets, sonus.buffx" };

    buffer_reference m_buffer
    {
        this,
        MIN_FUNCTION
        {
            return {};
        }
    };

    argument<symbol> m_source_arg
    {
        this,
        "buffer",
        "buffer~ to work on.",
        MIN_ARGUMENT_FUNCTION
        {
            m_buffer.set(arg);
        }
    };

    argument<int> snaps_arg
    {
        this,
        "snapshots",
        "The number of slots available for snapshots."
    };

    bufsnap(const atoms& args = {})
    {
        int snaps = 1;

        if (!args.empty())
        {
            m_buffer.set(args[0]);
        }

        if (args.size() > 1 && int(args[1]) > 1)
        {
            snaps = std::min(int(args[1]), 16);
        }

	    for (auto i = 0; i < snaps; ++i)
        {
            m_inlets_.push_back(std::make_unique<inlet<>>(this, "(symbol) Messages for snapshot " + std::to_string(i+1)));
        }
    }

    message<> snap
    {
        this,
        "snap",
        "Save the current buffer content",
        setter
        {
            MIN_FUNCTION
            {
                int slot = inlet;

                if (inlet == 0 && !args.empty())
                {
                    if (int(args[0]) >= 1 && int(args[0]) <= m_inlets_.size())
                    {
                        slot = int(args[0]) - 1;
                    }
                }

                save_buffer(slot);
                return {};
            }
        }
    };

    message<> restore
    {
        this,
        "restore",
        "Restore the selected snapshot",
        MIN_FUNCTION
        {
            int slot = inlet;

            if (inlet == 0 && !args.empty())
            {
                if (int(args[0]) >= 1 && int(args[0]) <= m_inlets_.size())
                {
                    slot = int(args[0]) - 1;
                }
            }

            buffer_lock<false> b(m_buffer);

            if (b.valid() && snapshots_.at(slot).size() > 0)
            {
                b.resize_in_samples(snapshots_.at(slot).at(0).size());

                b.dirty();

                buffer_lock<> b_new(m_buffer);

                if (b_new.valid())
                {
                    for (auto ch = 0; ch < b_new.channel_count(); ch++)
                    {
                        if (snapshots_.at(slot).size() > ch)
                        {
                            for (auto s = 0; s < b_new.frame_count(); s++)
                            {
                                if (snapshots_.at(slot).at(ch).size() > s)
                                {
                                    b_new.lookup(s, ch) = snapshots_.at(slot).at(ch).at(s);
                                }
                            }
                        }
                    }

                    b_new.dirty();
                }
            }
            
            return {};
        }
    };

private:
    std::array<Snapshot, 16> snapshots_;
    std::vector<std::unique_ptr<inlet<>>> m_inlets_;

    void save_buffer(const int &slot)
    {
        if (slot >= snapshots_.size())
        {
            return;
        }

        buffer_lock<> b(m_buffer);

        if (b.valid())
        {
            Snapshot new_snapshot;

            for (int ch = 0; ch < b.channel_count(); ch++)
            {
                std::vector<double> curr_channel(b.frame_count());

                for (int s = 0; s < b.frame_count(); s++)
                {
                    curr_channel.at(s) = b.lookup(s, ch);
                }

                new_snapshot.push_back(curr_channel);
            }

            snapshots_.at(slot) = new_snapshot;
        }
    }
};

MIN_EXTERNAL(bufsnap);
