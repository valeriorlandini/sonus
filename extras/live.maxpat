{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 8,
			"minor" : 6,
			"revision" : 1,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"classnamespace" : "box",
		"rect" : [ 35.0, 84.0, 1245.0, 979.0 ],
		"bglocked" : 0,
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 1,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 1,
		"objectsnaponopen" : 1,
		"statusbarvisible" : 2,
		"toolbarvisible" : 1,
		"lefttoolbarpinned" : 0,
		"toptoolbarpinned" : 0,
		"righttoolbarpinned" : 0,
		"bottomtoolbarpinned" : 0,
		"toolbars_unpinned_last_save" : 0,
		"tallnewobj" : 0,
		"boxanimatetime" : 200,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"description" : "",
		"digest" : "",
		"tags" : "",
		"style" : "",
		"subpatcher_template" : "",
		"assistshowspatchername" : 0,
		"boxes" : [ 			{
				"box" : 				{
					"autosave" : 1,
					"bgmode" : 1,
					"border" : 0,
					"clickthrough" : 0,
					"enablehscroll" : 0,
					"enablevscroll" : 0,
					"id" : "obj-9",
					"lockeddragscroll" : 0,
					"lockedsize" : 0,
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 4,
					"offset" : [ 0.0, 0.0 ],
					"outlettype" : [ "signal", "signal", "", "" ],
					"patching_rect" : [ 23.0, 14.0, 415.0, 196.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_invisible" : 1,
							"parameter_longname" : "amxd~[2]",
							"parameter_modmode" : 0,
							"parameter_shortname" : "amxd~[2]",
							"parameter_type" : 3
						}

					}
,
					"saved_object_attributes" : 					{
						"parameter_enable" : 1,
						"patchername" : "Bitman.amxd",
						"patchername_fallback" : "~/Documents/Max 8/Packages/sonus/patchers/Max Audio Effect/Bitman.amxd"
					}
,
					"snapshot" : 					{
						"filetype" : "C74Snapshot",
						"version" : 2,
						"minorversion" : 0,
						"name" : "snapshotlist",
						"origin" : "max~",
						"type" : "list",
						"subtype" : "Undefined",
						"embed" : 1,
						"snapshot" : 						{
							"name" : "Bitman.amxd",
							"origname" : "~/Documents/Max 8/Packages/sonus/patchers/Max Audio Effect/Bitman.amxd",
							"valuedictionary" : 							{
								"parameter_values" : 								{
									"Bit 1 State" : 0.0,
									"Bit 2 State" : 2.0,
									"Bit 3 State" : 2.0,
									"Bit 4 State" : 2.0,
									"Bit 5 State" : 3.0,
									"Bit 6 State" : 2.0,
									"Bit 7 State" : 1.0,
									"Bit 8 State" : 0.0,
									"DC Filter" : 1.0,
									"Gain" : -3.514718625761429,
									"Gate Threshold" : -30.0,
									"Lowpass Cutoff" : 20000.0,
									"Randomize Bits" : 0.0,
									"Reset Bits" : 0.0,
									"Wet" : 100.0
								}

							}
,
							"active" : 1
						}
,
						"snapshotlist" : 						{
							"current_snapshot" : 0,
							"entries" : [ 								{
									"filetype" : "C74Snapshot",
									"version" : 2,
									"minorversion" : 0,
									"name" : "Bitman.amxd",
									"origin" : "Bitman.amxd",
									"type" : "amxd",
									"subtype" : "Undefined",
									"embed" : 0,
									"snapshot" : 									{
										"name" : "Bitman.amxd",
										"origname" : "~/Documents/Max 8/Packages/sonus/patchers/Max Audio Effect/Bitman.amxd",
										"valuedictionary" : 										{
											"parameter_values" : 											{
												"Bit 1 State" : 0.0,
												"Bit 2 State" : 2.0,
												"Bit 3 State" : 2.0,
												"Bit 4 State" : 2.0,
												"Bit 5 State" : 3.0,
												"Bit 6 State" : 2.0,
												"Bit 7 State" : 1.0,
												"Bit 8 State" : 0.0,
												"DC Filter" : 1.0,
												"Gain" : -3.514718625761429,
												"Gate Threshold" : -30.0,
												"Lowpass Cutoff" : 20000.0,
												"Randomize Bits" : 0.0,
												"Reset Bits" : 0.0,
												"Wet" : 100.0
											}

										}
,
										"active" : 1
									}
,
									"fileref" : 									{
										"name" : "Bitman.amxd",
										"filename" : "Bitman.amxd.maxsnap",
										"filepath" : "~/Documents/Max 8/Snapshots",
										"filepos" : -1,
										"snapshotfileid" : "4c6b68eaff92f058ea1048f14e781371"
									}

								}
 ]
						}

					}
,
					"text" : "amxd~ bitman",
					"varname" : "bitman",
					"viewvisibility" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-8",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 626.5, 14.0, 180.0, 34.0 ],
					"presentation_linecount" : 2,
					"text" : "A bitcrusher with the ability to manipulate the single bits"
				}

			}
, 			{
				"box" : 				{
					"hidden" : 1,
					"id" : "obj-5",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 817.0, 19.0, 70.0, 22.0 ],
					"text" : "loadmess 0"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-4",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 627.0, 89.0, 180.0, 34.0 ],
					"text" : "A pulsar synthesis based subtractive synthesizer"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-3",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 626.0, 52.0, 181.0, 34.0 ],
					"text" : "A bytebeat generator and LFO, with a bonus reverb"
				}

			}
, 			{
				"box" : 				{
					"hidden" : 1,
					"id" : "obj-103",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 542.0, 186.0, 67.0, 22.0 ],
					"save" : [ "#N", "thispatcher", ";", "#Q", "end", ";" ],
					"text" : "thispatcher"
				}

			}
, 			{
				"box" : 				{
					"hidden" : 1,
					"id" : "obj-59",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 6,
							"revision" : 1,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 130.0, 102.0, 1752.0, 671.0 ],
						"bglocked" : 0,
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 1,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 1,
						"objectsnaponopen" : 1,
						"statusbarvisible" : 2,
						"toolbarvisible" : 1,
						"lefttoolbarpinned" : 0,
						"toptoolbarpinned" : 0,
						"righttoolbarpinned" : 0,
						"bottomtoolbarpinned" : 0,
						"toolbars_unpinned_last_save" : 0,
						"tallnewobj" : 0,
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "",
						"assistshowspatchername" : 0,
						"boxes" : [ 							{
								"box" : 								{
									"id" : "obj-5",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 45.0, 202.0, 207.0, 22.0 ],
									"presentation_linecount" : 2,
									"text" : "script sendbox bitman_text hidden $1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-6",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 45.0, 123.0, 29.5, 22.0 ],
									"text" : "!= 0"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-7",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 45.0, 173.0, 181.0, 22.0 ],
									"presentation_linecount" : 2,
									"text" : "script sendbox bitman hidden $1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-3",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 492.0, 202.0, 223.0, 22.0 ],
									"text" : "script sendbox pulsynthe_text hidden $1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-2",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 290.0, 202.0, 198.0, 22.0 ],
									"text" : "script sendbox byter_text hidden $1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-4",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 492.0, 173.0, 197.0, 22.0 ],
									"text" : "script sendbox pulsynthe hidden $1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-50",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 492.0, 123.0, 29.5, 22.0 ],
									"text" : "!= 2"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-48",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 290.0, 123.0, 29.5, 22.0 ],
									"text" : "!= 1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-43",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 290.0, 173.0, 172.0, 22.0 ],
									"text" : "script sendbox byter hidden $1"
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-24",
									"index" : 1,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 45.0, 258.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-1",
									"index" : 1,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 44.75, 45.0, 30.0, 30.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-48", 0 ],
									"order" : 1,
									"source" : [ "obj-1", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-50", 0 ],
									"order" : 0,
									"source" : [ "obj-1", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-6", 0 ],
									"order" : 2,
									"source" : [ "obj-1", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-24", 0 ],
									"source" : [ "obj-2", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-24", 0 ],
									"source" : [ "obj-3", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-24", 0 ],
									"source" : [ "obj-4", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-24", 0 ],
									"source" : [ "obj-43", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-2", 0 ],
									"order" : 0,
									"source" : [ "obj-48", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-43", 0 ],
									"order" : 1,
									"source" : [ "obj-48", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-24", 0 ],
									"source" : [ "obj-5", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-3", 0 ],
									"order" : 0,
									"source" : [ "obj-50", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-4", 0 ],
									"order" : 1,
									"source" : [ "obj-50", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-5", 0 ],
									"order" : 0,
									"source" : [ "obj-6", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-7", 0 ],
									"order" : 1,
									"source" : [ "obj-6", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-24", 0 ],
									"source" : [ "obj-7", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 542.0, 155.0, 63.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p switcher"
				}

			}
, 			{
				"box" : 				{
					"contrastactivetab" : 0,
					"fontface" : 1,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"htabcolor" : [ 1.0, 0.392156862745098, 0.0, 1.0 ],
					"id" : "obj-97",
					"maxclass" : "tab",
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "int", "", "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 542.0, 14.0, 82.0, 110.0 ],
					"spacing_x" : 1.0,
					"tabcolor" : [ 0.427450980392157, 0.843137254901961, 1.0, 1.0 ],
					"tabs" : [ "Bitman", "Byter", "Pulsynthe" ],
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"truncate" : 0
				}

			}
, 			{
				"box" : 				{
					"autosave" : 1,
					"bgmode" : 1,
					"border" : 0,
					"clickthrough" : 0,
					"enablehscroll" : 0,
					"enablevscroll" : 0,
					"hidden" : 1,
					"id" : "obj-2",
					"lockeddragscroll" : 0,
					"lockedsize" : 0,
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 4,
					"offset" : [ 0.0, 0.0 ],
					"outlettype" : [ "signal", "signal", "", "" ],
					"patching_rect" : [ 23.0, 14.0, 483.0, 196.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_invisible" : 1,
							"parameter_longname" : "amxd~[1]",
							"parameter_modmode" : 0,
							"parameter_shortname" : "amxd~[1]",
							"parameter_type" : 3
						}

					}
,
					"saved_object_attributes" : 					{
						"parameter_enable" : 1,
						"patchername" : "Pulsynthe.amxd",
						"patchername_fallback" : "~/Documents/Max 8/Packages/sonus/patchers/Max Instrument/Pulsynthe.amxd"
					}
,
					"snapshot" : 					{
						"filetype" : "C74Snapshot",
						"version" : 2,
						"minorversion" : 0,
						"name" : "snapshotlist",
						"origin" : "max~",
						"type" : "list",
						"subtype" : "Undefined",
						"embed" : 1,
						"snapshot" : 						{
							"name" : "Pulsynthe.amxd",
							"origname" : "~/Documents/Max 8/Packages/sonus/patchers/Max Instrument/Pulsynthe.amxd",
							"valuedictionary" : 							{
								"parameter_values" : 								{
									"Filter Cutoff" : 20000.0,
									"Filter Envelope Amount" : 0.0,
									"Filter Resonance" : 50.0,
									"Filter Type" : 0.0,
									"Master Gain" : -15.304526898964873,
									"Oscillator 1 Duty Cycle" : 50.0,
									"Oscillator 1 Duty Cycle Mod Amount" : 0.0,
									"Oscillator 1 Duty Cycle Mod Frequency" : 0.1,
									"Oscillator 1 Frequency Divider" : 0.0,
									"Oscillator 1 Waveform" : 2.0,
									"Oscillator 1 Window" : 0.0,
									"Oscillator 2 Detune" : 0.0,
									"Oscillator 2 Duty Cycle" : 50.0,
									"Oscillator 2 Duty Cycle Mod Amount" : 0.0,
									"Oscillator 2 Duty Cycle Mod Frequency" : 0.1,
									"Oscillator 2 Frequency Divider" : 0.0,
									"Oscillator 2 Waveform" : 3.0,
									"Oscillator 2 Window" : 0.0,
									"Oscillators Mix" : 50.0,
									"Pitch Envelope Amount" : 0.0,
									"Synthesis Algorithm" : 0.0,
									"blob" : 									{
										"ADSR Selector" : [ 0 ]
									}

								}

							}
,
							"active" : 1
						}
,
						"snapshotlist" : 						{
							"current_snapshot" : 0,
							"entries" : [ 								{
									"filetype" : "C74Snapshot",
									"version" : 2,
									"minorversion" : 0,
									"name" : "Pulsynthe.amxd",
									"origin" : "Pulsynthe.amxd",
									"type" : "amxd",
									"subtype" : "Undefined",
									"embed" : 0,
									"snapshot" : 									{
										"name" : "Pulsynthe.amxd",
										"origname" : "~/Documents/Max 8/Packages/sonus/patchers/Max Instrument/Pulsynthe.amxd",
										"valuedictionary" : 										{
											"parameter_values" : 											{
												"Filter Cutoff" : 20000.0,
												"Filter Envelope Amount" : 0.0,
												"Filter Resonance" : 50.0,
												"Filter Type" : 0.0,
												"Master Gain" : -15.304526898964873,
												"Oscillator 1 Duty Cycle" : 50.0,
												"Oscillator 1 Duty Cycle Mod Amount" : 0.0,
												"Oscillator 1 Duty Cycle Mod Frequency" : 0.1,
												"Oscillator 1 Frequency Divider" : 0.0,
												"Oscillator 1 Waveform" : 2.0,
												"Oscillator 1 Window" : 0.0,
												"Oscillator 2 Detune" : 0.0,
												"Oscillator 2 Duty Cycle" : 50.0,
												"Oscillator 2 Duty Cycle Mod Amount" : 0.0,
												"Oscillator 2 Duty Cycle Mod Frequency" : 0.1,
												"Oscillator 2 Frequency Divider" : 0.0,
												"Oscillator 2 Waveform" : 3.0,
												"Oscillator 2 Window" : 0.0,
												"Oscillators Mix" : 50.0,
												"Pitch Envelope Amount" : 0.0,
												"Synthesis Algorithm" : 0.0,
												"blob" : 												{
													"ADSR Selector" : [ 0 ]
												}

											}

										}
,
										"active" : 1
									}
,
									"fileref" : 									{
										"name" : "Pulsynthe.amxd",
										"filename" : "Pulsynthe.amxd.maxsnap",
										"filepath" : "~/Documents/Max 8/Snapshots",
										"filepos" : -1,
										"snapshotfileid" : "018b3ac10c6f44c26db5dcdc8d6004bb"
									}

								}
 ]
						}

					}
,
					"text" : "amxd~ pulsynthe",
					"varname" : "pulsynthe",
					"viewvisibility" : 1
				}

			}
, 			{
				"box" : 				{
					"autosave" : 1,
					"bgmode" : 1,
					"border" : 0,
					"clickthrough" : 0,
					"enablehscroll" : 0,
					"enablevscroll" : 0,
					"hidden" : 1,
					"id" : "obj-1",
					"lockeddragscroll" : 0,
					"lockedsize" : 0,
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 4,
					"offset" : [ 0.0, 0.0 ],
					"outlettype" : [ "signal", "signal", "", "" ],
					"patching_rect" : [ 23.0, 14.0, 500.5, 196.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_invisible" : 1,
							"parameter_longname" : "amxd~",
							"parameter_modmode" : 0,
							"parameter_shortname" : "amxd~",
							"parameter_type" : 3
						}

					}
,
					"saved_object_attributes" : 					{
						"parameter_enable" : 1,
						"patchername" : "Byter.amxd",
						"patchername_fallback" : "~/Documents/Max 8/Packages/sonus/patchers/Max Audio Effect/Byter.amxd"
					}
,
					"snapshot" : 					{
						"filetype" : "C74Snapshot",
						"version" : 2,
						"minorversion" : 0,
						"name" : "snapshotlist",
						"origin" : "max~",
						"type" : "list",
						"subtype" : "Undefined",
						"embed" : 1,
						"snapshot" : 						{
							"name" : "Byter.amxd",
							"origname" : "~/Documents/Max 8/Packages/sonus/patchers/Max Audio Effect/Byter.amxd",
							"valuedictionary" : 							{
								"parameter_values" : 								{
									"Bytebeat 1 SR Reduction" : 4.0,
									"Bytebeat 2 SR Reduction" : 4.0,
									"Dronize" : 0.0,
									"Gain" : -22.918903048616571,
									"Generators Balance" : 50.0,
									"Infrasonic" : 0.0,
									"Pass Audio In" : 1.0,
									"blob" : 									{
										"Bytebeat 1 Formula" : [ "(t&t%255)-(t>>13&(t%(t>>8|t>>16)))" ],
										"Bytebeat 2 Formula" : [ "t*(t>>10&((t>>16)+1))" ],
										"Preset" : [ 5 ],
										"Show Generator" : [ 1 ],
										"TargetMax[1]" : [ 100 ],
										"TargetMax[5]" : [ 100 ],
										"TargetMax[6]" : [ 100 ],
										"TargetMin[1]" : [ 0 ],
										"TargetMin[5]" : [ 0 ],
										"TargetMin[6]" : [ 0 ]
									}

								}

							}
,
							"active" : 1
						}
,
						"snapshotlist" : 						{
							"current_snapshot" : 0,
							"entries" : [ 								{
									"filetype" : "C74Snapshot",
									"version" : 2,
									"minorversion" : 0,
									"name" : "Byter.amxd",
									"origin" : "Byter.amxd",
									"type" : "amxd",
									"subtype" : "Undefined",
									"embed" : 0,
									"snapshot" : 									{
										"name" : "Byter.amxd",
										"origname" : "~/Documents/Max 8/Packages/sonus/patchers/Max Audio Effect/Byter.amxd",
										"valuedictionary" : 										{
											"parameter_values" : 											{
												"Bytebeat 1 SR Reduction" : 4.0,
												"Bytebeat 2 SR Reduction" : 4.0,
												"Dronize" : 0.0,
												"Gain" : -22.918903048616571,
												"Generators Balance" : 50.0,
												"Infrasonic" : 0.0,
												"Pass Audio In" : 1.0,
												"blob" : 												{
													"Bytebeat 1 Formula" : [ "(t&t%255)-(t>>13&(t%(t>>8|t>>16)))" ],
													"Bytebeat 2 Formula" : [ "t*(t>>10&((t>>16)+1))" ],
													"Preset" : [ 5 ],
													"Show Generator" : [ 1 ],
													"TargetMax[1]" : [ 100 ],
													"TargetMax[5]" : [ 100 ],
													"TargetMax[6]" : [ 100 ],
													"TargetMin[1]" : [ 0 ],
													"TargetMin[5]" : [ 0 ],
													"TargetMin[6]" : [ 0 ]
												}

											}

										}
,
										"active" : 1
									}
,
									"fileref" : 									{
										"name" : "Byter.amxd",
										"filename" : "Byter.amxd.maxsnap",
										"filepath" : "~/Documents/Max 8/Snapshots",
										"filepos" : -1,
										"snapshotfileid" : "a7de2654e53afd2cd4a0deab38173f05"
									}

								}
 ]
						}

					}
,
					"text" : "amxd~ byter",
					"varname" : "byter",
					"viewvisibility" : 1
				}

			}
, 			{
				"box" : 				{
					"background" : 1,
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 16.0,
					"id" : "obj-10",
					"linecount" : 3,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 23.0, 228.0, 805.0, 62.0 ],
					"presentation_linecount" : 3,
					"text" : "Bitman is an effect that allows to manipulate the single 8 bits of the incoming bitreduced signal. Each bit can be kept as it is, inverted (toggle), set to 1 (set) or set to 0 (clear). Additional controls allow for lowpass filtering, removing the DC offset (suggested to be kept always on) and randomizing the bits statuses",
					"textcolor" : [ 0.258823529411765, 0.258823529411765, 0.258823529411765, 1.0 ],
					"varname" : "bitman_text"
				}

			}
, 			{
				"box" : 				{
					"background" : 1,
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 16.0,
					"hidden" : 1,
					"id" : "obj-7",
					"linecount" : 3,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 23.0, 228.0, 805.0, 62.0 ],
					"text" : "Pulsynthe is a simple but complete subtractive synthesizer based on pulsar synthesis. The two oscillators can have their pulsar synthesis parameters separately adjusted. Three envelopes and four synthesis modes (mix, ring modulation, bitwise or, bitwise and) allow for a wide range of sounds and modulations.",
					"textcolor" : [ 0.258823529411765, 0.258823529411765, 0.258823529411765, 1.0 ],
					"varname" : "pulsynthe_text"
				}

			}
, 			{
				"box" : 				{
					"background" : 1,
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 16.0,
					"hidden" : 1,
					"id" : "obj-6",
					"linecount" : 4,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 23.0, 228.0, 805.0, 80.0 ],
					"text" : "Byter is a bytebeat generator, with the possibility to use the generated signal as modulator. This is made more effective by the Infrasonic switch, that slows down the generation speed so that the produced signal gradually changes. Moreover, a Dronize knob allows to add a thick reverberation, turning this device in an ever evolving drone and soundscape generator.",
					"textcolor" : [ 0.258823529411765, 0.258823529411765, 0.258823529411765, 1.0 ],
					"varname" : "byter_text"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-97", 0 ],
					"hidden" : 1,
					"source" : [ "obj-5", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-103", 0 ],
					"hidden" : 1,
					"source" : [ "obj-59", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-59", 0 ],
					"hidden" : 1,
					"source" : [ "obj-97", 0 ]
				}

			}
 ]
	}

}
