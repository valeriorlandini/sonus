{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 9,
            "minor": 1,
            "revision": 1,
            "architecture": "x64",
            "modernui": 1
        },
        "classnamespace": "box",
        "rect": [ 486.0, 246.0, 868.0, 651.0 ],
        "boxes": [
            {
                "box": {
                    "id": "obj-12",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 23.0, 253.0, 249.0, 20.0 ],
                    "presentation_linecount": 2,
                    "text": "Collatz conjecture based wavetable oscillator",
                    "textcolor": [ 0.850980392156863, 0.850980392156863, 0.850980392156863, 1.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-13",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 26.0, 232.0, 85.0, 22.0 ],
                    "text": "sonus.collatz~"
                }
            },
            {
                "box": {
                    "id": "obj-10",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 278.0, 163.0, 233.0, 20.0 ],
                    "text": "Scrambled wavetable oscillator",
                    "textcolor": [ 0.850980392156863, 0.850980392156863, 0.850980392156863, 1.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-11",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 281.0, 142.0, 92.0, 22.0 ],
                    "text": "sonus.nowave~"
                }
            },
            {
                "box": {
                    "id": "obj-2",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 23.0, 343.0, 233.0, 20.0 ],
                    "text": "Harmonic additive generator",
                    "textcolor": [ 0.850980392156863, 0.850980392156863, 0.850980392156863, 1.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-8",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 26.0, 322.0, 97.0, 22.0 ],
                    "text": "sonus.harmosc~"
                }
            },
            {
                "box": {
                    "id": "obj-6",
                    "maxclass": "live.line",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 16.0, 113.0, 805.0, 5.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-1",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 23.0, 208.0, 233.0, 20.0 ],
                    "text": "Bytebeat player",
                    "textcolor": [ 0.850980392156863, 0.850980392156863, 0.850980392156863, 1.0 ]
                }
            },
            {
                "box": {
                    "comment": "",
                    "hidden": 1,
                    "id": "obj-3",
                    "index": 0,
                    "maxclass": "outlet",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 26.0, 641.0, 30.0, 30.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-93",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 279.0, 253.0, 233.0, 20.0 ],
                    "text": "Phase distortion based oscillator",
                    "textcolor": [ 0.850980392156863, 0.850980392156863, 0.850980392156863, 1.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-94",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 282.0, 232.0, 83.0, 22.0 ],
                    "text": "sonus.pdosc~"
                }
            },
            {
                "box": {
                    "id": "obj-86",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 26.0, 187.0, 96.0, 22.0 ],
                    "text": "sonus.byteplay~"
                }
            },
            {
                "box": {
                    "id": "obj-84",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 23.0, 163.0, 233.0, 20.0 ],
                    "text": "Bytebeat generator",
                    "textcolor": [ 0.850980392156863, 0.850980392156863, 0.850980392156863, 1.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-85",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 26.0, 142.0, 97.0, 22.0 ],
                    "text": "sonus.bytebeat~"
                }
            },
            {
                "box": {
                    "id": "obj-82",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 23.0, 433.0, 233.0, 20.0 ],
                    "text": "Neural network based oscillator",
                    "textcolor": [ 0.850980392156863, 0.850980392156863, 0.850980392156863, 1.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-83",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 26.0, 412.0, 94.0, 22.0 ],
                    "text": "sonus.neurosc~"
                }
            },
            {
                "box": {
                    "id": "obj-80",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 23.0, 298.0, 233.0, 20.0 ],
                    "text": "Elementary cellular automata oscillator",
                    "textcolor": [ 0.850980392156863, 0.850980392156863, 0.850980392156863, 1.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-81",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 26.0, 277.0, 89.0, 22.0 ],
                    "text": "sonus.ecaosc~"
                }
            },
            {
                "box": {
                    "id": "obj-55",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 23.0, 388.0, 233.0, 20.0 ],
                    "text": "Lorenz attractor based generator",
                    "textcolor": [ 0.850980392156863, 0.850980392156863, 0.850980392156863, 1.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-56",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 26.0, 367.0, 84.0, 22.0 ],
                    "text": "sonus.lorenz~"
                }
            },
            {
                "box": {
                    "id": "obj-47",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 280.0, 298.0, 233.0, 20.0 ],
                    "text": "Pulsar oscillator",
                    "textcolor": [ 0.850980392156863, 0.850980392156863, 0.850980392156863, 1.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-48",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 283.0, 277.0, 84.0, 22.0 ],
                    "text": "sonus.pulsar~"
                }
            },
            {
                "box": {
                    "id": "obj-41",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 280.0, 388.0, 233.0, 20.0 ],
                    "text": "Oscillator with random shapes",
                    "textcolor": [ 0.850980392156863, 0.850980392156863, 0.850980392156863, 1.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-42",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 283.0, 367.0, 80.0, 22.0 ],
                    "text": "sonus.rsosc~"
                }
            },
            {
                "box": {
                    "id": "obj-37",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 280.0, 433.0, 233.0, 20.0 ],
                    "text": "Multishape bandlimited oscillator",
                    "textcolor": [ 0.850980392156863, 0.850980392156863, 0.850980392156863, 1.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-38",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 283.0, 412.0, 82.0, 22.0 ],
                    "text": "sonus.vaosc~"
                }
            },
            {
                "box": {
                    "id": "obj-23",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 280.0, 343.0, 233.0, 20.0 ],
                    "text": "Rössler attractor based generator",
                    "textcolor": [ 0.850980392156863, 0.850980392156863, 0.850980392156863, 1.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-7",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 278.0, 208.0, 233.0, 20.0 ],
                    "text": "User defined wavetable oscillator",
                    "textcolor": [ 0.850980392156863, 0.850980392156863, 0.850980392156863, 1.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-4",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 533.0, 163.0, 233.0, 20.0 ],
                    "text": "Window functions generator",
                    "textcolor": [ 0.850980392156863, 0.850980392156863, 0.850980392156863, 1.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-15",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 283.0, 322.0, 94.0, 22.0 ],
                    "text": "sonus.roessler~"
                }
            },
            {
                "box": {
                    "id": "obj-14",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 281.0, 187.0, 85.0, 22.0 ],
                    "text": "sonus.nwosc~"
                }
            },
            {
                "box": {
                    "id": "obj-9",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 536.0, 142.0, 91.0, 22.0 ],
                    "text": "sonus.window~"
                }
            },
            {
                "box": {
                    "background": 1,
                    "fontface": 0,
                    "fontname": "Arial",
                    "fontsize": 16.0,
                    "id": "obj-5",
                    "linecount": 4,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 16.0, 23.0, 805.0, 80.0 ],
                    "text": "A series of externals that generate signal through different algorithms, from bytebeat generators to strange attractors. There are also more traditional oscillators, but with some bonus features that may save you patch cables compared to similar standard MSP cables.\nMost of these externals are suitable as sources of both audio and modulation signals.",
                    "textcolor": [ 0.956862745098039, 0.764705882352941, 0.450980392156863, 1.0 ]
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "destination": [ "obj-3", 0 ],
                    "hidden": 1,
                    "source": [ "obj-11", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 0 ],
                    "hidden": 1,
                    "source": [ "obj-13", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 0 ],
                    "hidden": 1,
                    "source": [ "obj-14", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 0 ],
                    "hidden": 1,
                    "source": [ "obj-15", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 0 ],
                    "hidden": 1,
                    "source": [ "obj-38", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 0 ],
                    "hidden": 1,
                    "source": [ "obj-42", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 0 ],
                    "hidden": 1,
                    "source": [ "obj-48", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 0 ],
                    "hidden": 1,
                    "source": [ "obj-56", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 0 ],
                    "hidden": 1,
                    "source": [ "obj-8", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 0 ],
                    "hidden": 1,
                    "source": [ "obj-81", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 0 ],
                    "hidden": 1,
                    "source": [ "obj-83", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 0 ],
                    "hidden": 1,
                    "source": [ "obj-85", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 0 ],
                    "hidden": 1,
                    "source": [ "obj-86", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 0 ],
                    "hidden": 1,
                    "source": [ "obj-9", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 0 ],
                    "hidden": 1,
                    "source": [ "obj-94", 0 ]
                }
            }
        ],
        "autosave": 0
    }
}