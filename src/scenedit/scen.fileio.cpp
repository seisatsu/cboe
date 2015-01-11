
#include <cstring>
#include "scen.global.h"
#include "classes.h"
#include <iostream>
#include <fstream>
#include "scen.fileio.h"
#include "scen.keydlgs.h"
#include "graphtool.hpp"
#include "scen.core.h"
#include "soundtool.hpp"
#include "mathutil.hpp"
#include "oldstructs.h"
#include "fileio.hpp"
#include "dlogutil.hpp"
#include "tarball.hpp"
#include "gzstream.h"

#define	DONE_BUTTON_ITEM	1

extern cScenario scenario;

extern cTown* town;
extern short cur_town;//overall_mode,given_password,user_given_password;
extern location cur_out;
//extern piles_of_stuff_dumping_type *data_store;
//extern cSpeech talking;
extern cOutdoors* current_terrain;
extern bool change_made;
extern cCustomGraphics spec_scen_g;
extern bool mac_is_intel;

short specials_res_id,data_dump_file_id;
char start_name[256];
short start_volume,data_volume,jl = 0;

fs::path temp_file_to_load;
std::string last_load_file = "Blades of Exile Scenario";
extern fs::path progDir;
extern bool cur_scen_is_mac;

void print_write_position ();
void load_spec_graphics();

template<typename Container> static void writeSpecialNodes(std::ostream& fout, Container nodes) {
	static_assert(std::is_same<typename Container::value_type, cSpecial>::value,
		"writeSpecialNodes must be instantiated with a container of special nodes");
	for(size_t i = 0; i < nodes.size(); i++) {
		nodes[i].writeTo(fout, i);
	}
}

void save_scenario(fs::path toFile) {
	// TODO: I'm not certain 1.0.0 is the correct version here?
	scenario.format.prog_make_ver[0] = 1;
	scenario.format.prog_make_ver[1] = 0;
	scenario.format.prog_make_ver[2] = 0;
	// TODO: This is just a skeletal outline of what needs to be done to save the scenario
	tarball scen_file;
	{
		// First, write out the scenario header data. This is in a binary format identical to older scenarios.
		std::ostream& header = scen_file.newFile("scenario/header.exs");
		
		// Next, the bulk scenario data.
		std::ostream& scen_data = scen_file.newFile("scenario/scenario.xml");
		
		// Then the terrains...
		std::ostream& terrain = scen_file.newFile("scenario/terrain.xml");
		
		// ...items...
		std::ostream& items = scen_file.newFile("scenario/items.xml");
		
		// ...and monsters
		std::ostream& monsters = scen_file.newFile("scenario/monsters.xml");
		
		// And the special nodes.
		std::ostream& scen_spec = scen_file.newFile("scenario/scenario.spec");
		writeSpecialNodes(scen_spec, scenario.scen_specials);
	}
	
	// Next, write the outdoors.
	for(size_t x = 0; x < scenario.outdoors.width(); x++) {
		for(size_t y = 0; y < scenario.outdoors.height(); y++) {
			std::string file_basename = "out" + std::to_string(x) + '~' + std::to_string(y);
			// First the main data.
			std::ostream& outdoors = scen_file.newFile("scenario/out/" + file_basename + ".xml");
			
			// Then the map.
			std::ostream& out_map = scen_file.newFile("scenario/out/" + file_basename + ".map");
			
			// And the special nodes.
			std::ostream& out_spec = scen_file.newFile("scenario/out/" + file_basename + ".spec");
			writeSpecialNodes(out_spec, scenario.outdoors[x][y]->specials);
		}
	}
	
	// And finally, the towns.
	for(size_t i = 0; i < scenario.towns.size(); i++) {
		std::string file_basename = 't' + std::to_string(i);
		// First the main data.
		std::ostream& town = scen_file.newFile("scenario/towns/" + file_basename + ".xml");
		
		// Then the map.
		std::ostream& town_map = scen_file.newFile("scenario/towns/" + file_basename + ".map");
		
		// And the special nodes.
		std::ostream& town_spec = scen_file.newFile("scenario/towns/" + file_basename + ".spec");
		writeSpecialNodes(town_spec, scenario.towns[i]->specials);
		
		// Don't forget the dialogue nodes.
		std::ostream& town_talk = scen_file.newFile("scenario/towns/" + file_basename + "talk.xml");
	}
	giveError("Sorry, scenario saving is currently disabled.");
	return;
	
	// Make sure it has the proper file extension
	std::string fname = toFile.filename().string();
	size_t dot = fname.find_last_of('.');
	if(dot == std::string::npos || fname.substr(dot) != ".boes")
		fname += ".boes";
	toFile = toFile.parent_path()/fname;
	
	// Now write to zip file.
	ogzstream zout(toFile.string().c_str());
	scen_file.writeTo(zout);
	zout.close();
}

void create_basic_scenario() {
//	short i,j,k,num_outdoors;
//	FSSpec to_load,new_file,dummy_file;
//	NavReplyRecord reply;
//	short dummy_f;
//	char *buffer = NULL;
//	Size buf_len = 100000;
//	OSErr error;
//	short out_num;
//	long len;
//	char message[256] = "Select saved game:                                     ";
//	legacy::big_tr_type t_d;
//	legacy::ave_tr_type ave_t;
//	legacy::tiny_tr_type tiny_t;
//
//	//FSMakeFSSpec(start_volume,start_dir,"Blades of Exile Base",&new_file);
//
//	//FSpDelete(&new_file);
//	//error = FSpCreate(&new_file,'blx!','BETM',smSystemScript);
//	//OK. FIrst find out what file name we're working with, and make the dummy file
//	// which we'll build the new scenario in
//	//to_load = store_file_reply;
//	FSMakeFSSpec(start_volume,start_dir,"::::Blades of Exile Base",&dummy_file);
//	FSpDelete(&dummy_file);
//	error = FSpCreate(&dummy_file,'blx!','BETM',reply.keyScript);
//	if((error != 0) && (error != dupFNErr)) {
//				//add_string_to_buf("Save: Couldn't create file.         ");
//				SysBeep(2);
//				return;
//				}
//	if((error = FSpOpenDF(&dummy_file,3,&dummy_f)) != 0) {
//		SysBeep(2);
//		return;
//		}
//
//
//	// Now we need to set up a buffer for moving the data over to the dummy
//	buffer = (char *) NewPtr(buf_len);
//	if(buffer == NULL) {
//		SysBeep(2); FSClose(dummy_f);
//		return;
//		}
//
//	cur_town = 0;
//	scenario.num_towns = 1;
//	scenario.town_size[0] = 1;
//	scenario.out_width = 1;
//	scenario.out_height = 1;
//	cur_out.x = 0;
//	cur_out.y = 0;
//	scenario.last_out_edited = cur_out;
//	// We're finally set up. Let's first set up the new scenario field
//	// We need the new info for the current town and outdoors, which may have been changed
//	scenario.town_data_size[cur_town][0] = sizeof(legacy::town_record_type);
//	if(scenario.town_size[cur_town] == 0)
//		scenario.town_data_size[cur_town][0] += sizeof(legacy::big_tr_type);
//		else if(scenario.town_size[cur_town] == 1)
//			scenario.town_data_size[cur_town][0] += sizeof(legacy::ave_tr_type);
//			else scenario.town_data_size[cur_town][0] += sizeof(legacy::tiny_tr_type);
//	scenario.town_data_size[cur_town][1] = 0;
//	for(i = 0; i < 60; i++)
//		scenario.town_data_size[cur_town][1] += strlen(data_store->town_strs[i]);
//	scenario.town_data_size[cur_town][2] = 0;
//	for(i = 60; i < 140; i++)
//		scenario.town_data_size[cur_town][2] += strlen(data_store->town_strs[i]);
//	scenario.town_data_size[cur_town][3] = sizeof(legacy::talking_record_type);
//	for(i = 0; i < 80; i++)
//		scenario.town_data_size[cur_town][3] += strlen(data_store->talk_strs[i]);
//	scenario.town_data_size[cur_town][4] = 0;
//	for(i = 80; i < 170; i++)
//		scenario.town_data_size[cur_town][4] += strlen(data_store->talk_strs[i]);
//
//	out_num = cur_out.y * scenario.out_width + cur_out.x;
//	scenario.out_data_size[out_num][0] = sizeof(legacy::outdoor_record_type);
//	scenario.out_data_size[out_num][1] = 0;
//	for(i = 0; i < 120; i++)
//		scenario.out_data_size[out_num][1] += strlen(data_store->out_strs[i]);
//
//	for(i = 0; i < 300; i++)
//		scenario.scen_str_len[i] = 0;
//	for(i = 0; i < 270; i++)
//		scenario.scen_str_len[i] = strlen(data_store->scen_strs[i]);
//	scenario.last_town_edited = cur_town;
//	scenario.last_out_edited = cur_out;
//
//	// now write scenario data
//	scenario.format.flag1 = 10;
//	scenario.format.flag2 = 20;
//	scenario.format.flag3 = 30;
//	scenario.format.flag4 = 40; /// these mean made on mac
//	len = sizeof(legacy::scenario_data_type); // scenario data
//	if((error = FSWrite(dummy_f, &len, (char *) &scenario)) != 0) {
//		SysBeep(2); FSClose(dummy_f);
//		return;
//		}
//	len = sizeof(scen_item_data_type); // item data
//	if((error = FSWrite(dummy_f, &len, (char *) &(data_store->scen_item_list))) != 0) {
//		SysBeep(2); FSClose(dummy_f);
//		return;
//		}
//	for(i = 0; i < 270; i++) { // scenario strings
//		len = (long) scenario.scen_str_len[i];
//		if((error = FSWrite(dummy_f, &len, (char *) &(data_store->scen_strs[i]))) != 0) {
//			SysBeep(2); FSClose(dummy_f);
//			return;
//			}
//		}
//
//	// OK ... scenario written. Now outdoors.
//	num_outdoors = scenario.out_width * scenario.out_height;
//	for(i = 0; i < num_outdoors; i++)
//		if(i == out_num) {
//			// write outdoors
//			for(j = 0; j < 180; j++)
//				current_terrain.strlens[j] = 0;
//			for(j = 0; j < 120; j++)
//				current_terrain.strlens[j] = strlen(data_store->out_strs[j]);
//			len = sizeof(legacy::outdoor_record_type);
//			FSWrite(dummy_f, &len, (char *) &current_terrain);
//
//			for(j = 0; j < 120; j++) {
//				len = (long) current_terrain.strlens[j];
//				FSWrite(dummy_f, &len, (char *) &(data_store->out_strs[j]));
//				}
//			}
//
//	// now, finally, write towns.
//	for(k = 0; k < scenario.num_towns; k++)
//		if(k == cur_town) {
//			for(i = 0; i < 180; i++)
//				town->strlens[i] = 0;
//			for(i = 0; i < 140; i++)
//				town->strlens[i] = strlen(data_store->town_strs[i]);
//
//			// write towns
//			len = sizeof(legacy::town_record_type);
//			FSWrite(dummy_f, &len, (char *) &town);
//
//			switch(scenario.town_size[cur_town]) {
//				case 0:
//					len = sizeof(legacy::big_tr_type);
//					FSWrite(dummy_f, &len, (char *) &t_d);
//					break;
//
//				case 1:
//					for(i = 0; i < 48; i++)
//						for(j = 0; j < 48; j++) {
//							ave_t.terrain[i][j] = town->terrain(i,j);
//							ave_t.lighting[i / 8][j] = town->lighting(i / 8,j);
//							}
//					for(i = 0; i < 16; i++) {
//						ave_t.room_rect[i] = town->room_rect(i);
//						}
//					for(i = 0; i < 40; i++) {
//						//ave_t.creatures[i] = town->creatures(i);
//						}
//					len = sizeof(legacy::ave_tr_type);
//					FSWrite(dummy_f, &len, (char *) &ave_t);
//				break;
//
//
//				case 2:
//					for(i = 0; i < 32; i++)
//						for(j = 0; j < 32; j++) {
//							tiny_t.terrain[i][j] = town->terrain(i,j);
//							tiny_t.lighting[i / 8][j] = town->lighting(i / 8,j);
//							}
//					for(i = 0; i < 16; i++) {
//						tiny_t.room_rect[i] = town->room_rect(i);
//						}
//					for(i = 0; i < 30; i++) {
//						//tiny_t.creatures[i] = town->creatures(i);
//						}
//					len = sizeof(legacy::tiny_tr_type);
//					FSWrite(dummy_f, &len, (char *) &tiny_t);
//					break;
//				}
//			for(j = 0; j < 140; j++) {
//				len = (long) town->strlens[j];
//				FSWrite(dummy_f, &len, (char *) &(data_store->town_strs[j]));
//				}
//
//			for(i = 0; i < 200; i++)
//				talking.strlens[i] = 0;
//			for(i = 0; i < 170; i++)
//				talking.strlens[i] = strlen(data_store->talk_strs[i]);
//			len = sizeof(legacy::talking_record_type);
//			FSWrite(dummy_f, &len, (char *) &talking);
//			for(j = 0; j < 170; j++) {
//				len = (long) talking.strlens[j];
//				FSWrite(dummy_f, &len, (char *) &(data_store->talk_strs[j]));
//				}
//
//			}
//
//
//	// now, everything is moved over. Delete the original, and rename the dummy
//	 FSClose(dummy_f);
//	DisposePtr(buffer);
	giveError("Scenario not created; creation currently disabled.");
}

// When this is called, the current town is the town to make town 0.
void make_new_scenario(std::string /*file_name*/,short /*out_width*/,short /*out_height*/,bool /*making_warriors_grove*/,
					   bool /*use_grass*/) {
//	short i,j,k,num_outdoors;
//	FSSpec dummy_file;
//	NavReplyRecord reply;
//	short dummy_f,file_id;
//	Size buf_len = 100000;
//	OSErr error;
//	short out_num;
//	long len,scen_ptr_move = 0,save_town_size = 0,save_out_size = 0;
//	location loc;
//	short x,y;
//	legacy::big_tr_type t_d;
//	legacy::ave_tr_type ave_t;
//	legacy::tiny_tr_type tiny_t;
//
//	// Step 1 - load scenario file from scenario base. It contains all the monsters and
//	// items done up properly!
//	error = FSMakeFSSpec(start_volume,start_dir,"::::Blades of Exile Base",&temp_file_to_load);
//	if(error != 0) {oops_error(80);}
//
//	if((error = FSpOpenDF(&temp_file_to_load,1,&file_id)) != 0) {
//		give_error("Could not find the file 'Blades of Exile Base', which needs to be in the Scenario Editor folder. If you no longer have it, you need to redownload Blades of Exile.","",0);
//		oops_error(81);	return;
//		}
//
//	len = (long) sizeof(legacy::scenario_data_type);
//	if((error = FSRead(file_id, &len, (char *) &scenario)) != 0){
//		FSClose(file_id); oops_error(82); return;
//		}
//
//	len = sizeof(scen_item_data_type); // item data
//	if((error = FSRead(file_id, &len, (char *) &(data_store->scen_item_list))) != 0){
//		FSClose(file_id); oops_error(83); return;
//		}
//	for(i = 0; i < 270; i++) {
//		len = (long) (scenario.scen_str_len[i]);
//		FSRead(file_id, &len, (char *) &(data_store->scen_strs[i]));
//		data_store->scen_strs[i][len] = 0;
//		}
//	FSClose(file_id);
//
//	char newname[256] = "::::";
//	strcat((char*)newname,(char*)file_name);
//
//	// now write scenario
//	c2pstr((char*) newname);
//
//	//OK. FIrst find out what file name we're working with, and make the dummy file
//	// which we'll build the new scenario in   Blades of Exile Base
//	FSMakeFSSpec(file_to_load.vRefNum,file_to_load.parID,newname,&dummy_file);
//	FSpDelete(&dummy_file);
//	error = FSpCreate(&dummy_file,'blx!','BETM',smSystemScript);
//	if((error != 0) && (error != dupFNErr)) {
//				oops_error(1);
//				return;
//				}
//	if((error = FSpOpenDF(&dummy_file,3,&dummy_f)) != 0) {
//		oops_error(2);
//		return;
//		}
//
//
//	scenario.format.prog_make_ver[0] = 1;
//	scenario.format.prog_make_ver[1] = 0;
//	scenario.format.prog_make_ver[2] = 0;
//	cur_town = 0;
//	scenario.num_towns = 1;
//	scenario.town_size[0] = 1;
//	scenario.out_width = out_width;
//	scenario.out_height = out_height;
//	cur_out.x = 0;
//	cur_out.y = 0;
//	scenario.last_out_edited = cur_out;
//
//	// We're finally set up. Let's first set up the new scenario field
//	// We need the new info for the current town and outdoors, which may have been changed
//	scenario.town_data_size[cur_town][0] = sizeof(legacy::town_record_type);
//	if(scenario.town_size[cur_town] == 0)
//		scenario.town_data_size[cur_town][0] += sizeof(legacy::big_tr_type);
//		else if(scenario.town_size[cur_town] == 1)
//			scenario.town_data_size[cur_town][0] += sizeof(legacy::ave_tr_type);
//			else scenario.town_data_size[cur_town][0] += sizeof(legacy::tiny_tr_type);
//	scenario.town_data_size[cur_town][1] = 0;
//	for(i = 0; i < 60; i++)
//		scenario.town_data_size[cur_town][1] += strlen(data_store->town_strs[i]);
//	scenario.town_data_size[cur_town][2] = 0;
//	for(i = 60; i < 140; i++)
//		scenario.town_data_size[cur_town][2] += strlen(data_store->town_strs[i]);
//	scenario.town_data_size[cur_town][3] = sizeof(legacy::talking_record_type);
//	for(i = 0; i < 80; i++)
//		scenario.town_data_size[cur_town][3] += strlen(data_store->talk_strs[i]);
//	scenario.town_data_size[cur_town][4] = 0;
//	for(i = 80; i < 170; i++)
//		scenario.town_data_size[cur_town][4] += strlen(data_store->talk_strs[i]);
//
//	num_outdoors = scenario.out_width * scenario.out_height;
//	for(i = 0; i < num_outdoors; i++) {
//		scenario.out_data_size[i][0] = sizeof(legacy::outdoor_record_type);
//		scenario.out_data_size[i][1] = 0;
//		for(j = 0; j < 120; j++)
//			scenario.out_data_size[i][1] += strlen(data_store->out_strs[j]);
//		}
//
//	for(i = 0; i < 300; i++)
//		scenario.scen_str_len[i] = 0;
//	for(i = 0; i < 270; i++)
//		scenario.scen_str_len[i] = strlen(data_store->scen_strs[i]);
//	scenario.last_town_edited = cur_town;
//	scenario.last_out_edited = cur_out;
//
//	// now write scenario data
//	scenario.format.flag1 = 10;
//	scenario.format.flag2 = 20;
//	scenario.format.flag3 = 30;
//	scenario.format.flag4 = 40; /// these mean made on mac
//	// now flags
//	scenario.flag_a = sizeof(legacy::scenario_data_type) + get_ran(1,-1000,1000);
//	scenario.flag_b = town_s(user_given_password);
//	scenario.flag_c = out_s(user_given_password);
//	scenario.flag_e = str_size_1(user_given_password);
//	scenario.flag_f = str_size_2(user_given_password);
//	scenario.flag_h = str_size_3(user_given_password);
//	scenario.flag_g = 10000 + get_ran(1,0,5000);
//	scenario.flag_d = init_data(user_given_password);
//
//	len = sizeof(legacy::scenario_data_type); // scenario data
//	scen_ptr_move += len;
//	if((error = FSWrite(dummy_f, &len, (char *) &scenario)) != 0) {
//		FSClose(dummy_f);
//				oops_error(3);
//		return;
//		}
//	len = sizeof(scen_item_data_type); // item data
//	scen_ptr_move += len;
//	if((error = FSWrite(dummy_f, &len, (char *) &(data_store->scen_item_list))) != 0) {
// 		FSClose(dummy_f);
//		oops_error(4);
//		return;
//		}
//	for(i = 0; i < 270; i++) { // scenario strings
//		len = (long) scenario.scen_str_len[i];
//		scen_ptr_move += len;
//		if((error = FSWrite(dummy_f, &len, (char *) &(scenario.scen_strs(i)))) != 0) {
//			FSClose(dummy_f);oops_error(5);
//			return;
//			}
//		}
//
//
//	// OK ... scenario written. Now outdoors.
//	num_outdoors = scenario.out_width * scenario.out_height;
//	for(i = 0; i < num_outdoors; i++)
//		{
//			loc.x = i % scenario.out_width;
//			loc.y = i / scenario.out_width;
//
//			for(x = 0; x < 48; x++)
//				for(y = 0; y < 48; y++) {
//					current_terrain.terrain[x][y] = (use_grass > 0) ? 2 : 0;
//
//					if((x < 4) && (loc.x == 0))
//						current_terrain.terrain[x][y] = (use_grass > 0) ? 22 : 5;
//					if((y < 4) && (loc.y == 0))
//						current_terrain.terrain[x][y] = (use_grass > 0) ? 22 : 5;
//					if((x > 43) && (loc.x == scenario.out_width - 1))
//						current_terrain.terrain[x][y] = (use_grass > 0) ? 22 : 5;
//					if((y > 43) && (loc.y == scenario.out_height - 1))
//						current_terrain.terrain[x][y] = (use_grass > 0) ? 22 : 5;
//					if((i == 0) && (making_warriors_grove > 0)) {
//						current_terrain.terrain[24][24] = (use_grass > 0) ? 234 : 232;
//						current_terrain.exit_locs[0].x = 24;
//						current_terrain.exit_locs[0].y = 24;
//						current_terrain.exit_dests[0] = 0;
//						}
//						else current_terrain.exit_locs[0].x = 100;
//					}
//
//			// write outdoors
//			for(j = 0; j < 180; j++)
//				current_terrain.strlens[j] = 0;
//			for(j = 0; j < 120; j++)
//				current_terrain.strlens[j] = strlen(current_terrain.out_strs(j));
//			len = sizeof(legacy::outdoor_record_type);
//			error = FSWrite(dummy_f, &len, (char *) &current_terrain);
//			if(error != 0) {FSClose(dummy_f);oops_error(6);}
//
//			for(j = 0; j < 120; j++) {
//				len = (long) current_terrain.strlens[j];
//				error = FSWrite(dummy_f, &len, (char *) &(current_terrain.out_strs(j)));
//				if(error != 0) {
//					FSClose(dummy_f);
//					oops_error(7);
//				}
//			}
//		}
//
//
//	// now, finally, write towns.
//	for(k = 0; k < scenario.num_towns; k++)
//		{
//			for(i = 0; i < 180; i++)
//				town->strlens[i] = 0;
//			for(i = 0; i < 140; i++)
//				town->strlens[i] = strlen(town->town_strs[i]);
//
//			// write towns
//			len = sizeof(legacy::town_record_type);
//			error = FSWrite(dummy_f, &len, (char *) &town);
//			if(error != 0) {FSClose(dummy_f);oops_error(8);}
//
//			switch(scenario.town_size[cur_town]) {
//				case 0:
//					len = sizeof(legacy::big_tr_type);
//					FSWrite(dummy_f, &len, (char *) &t_d);
//					break;
//
//				case 1:
//					for(i = 0; i < 48; i++)
//						for(j = 0; j < 48; j++) {
//							ave_t.terrain[i][j] = town->terrain(i,j);
//							ave_t.lighting[i / 8][j] = town->lighting(i / 8,j);
//							}
//					for(i = 0; i < 16; i++) {
//						ave_t.room_rect[i] = town->room_rect(i);
//						}
//					for(i = 0; i < 40; i++) {
//						//ave_t.creatures[i] = town->creatures(i);
//						}
//					len = sizeof(legacy::ave_tr_type);
//					FSWrite(dummy_f, &len, (char *) &ave_t);
//				break;
//
//
//				case 2:
//					for(i = 0; i < 32; i++)
//						for(j = 0; j < 32; j++) {
//							tiny_t.terrain[i][j] = town->terrain(i,j);
//							tiny_t.lighting[i / 8][j] = town->lighting(i / 8,j);
//							}
//					for(i = 0; i < 16; i++) {
//						tiny_t.room_rect[i] = town->room_rect(i);
//						}
//					for(i = 0; i < 30; i++) {
//						//tiny_t.creatures[i] = town->creatures(i);
//						}
//					len = sizeof(legacy::tiny_tr_type);
//					FSWrite(dummy_f, &len, (char *) &tiny_t);
//					break;
//				}
//			for(j = 0; j < 140; j++) {
//				len = (long) town->strlens[j];
//				FSWrite(dummy_f, &len, (char *) &(town->town_strs[j]));
//				}
//
//			for(i = 0; i < 200; i++)
//				talking.strlens[i] = 0;
//			for(i = 0; i < 170; i++)
//				talking.strlens[i] = strlen(town->talk_strs[i]);
//			len = sizeof(legacy::talking_record_type);
//			error = FSWrite(dummy_f, &len, (char *) &talking);
//			if(error != 0) {FSClose(dummy_f);oops_error(9);}
//			for(j = 0; j < 170; j++) {
//				len = (long) talking.strlens[j];
//				FSWrite(dummy_f, &len, (char *) &(town->talk_strs[j]));
//				}
//
//			}
//
//
//	change_made = true;
//	// now, everything is moved over. Delete the original, and rename the dummy
//	error = FSClose(dummy_f);
//	if(error != 0) {FSClose(dummy_f);oops_error(10);}
//	file_to_load = dummy_file;
	giveError("Scenario not created; creation is currently disabled.");
}

void start_data_dump() {
	short i;
	using std::endl;
	std::string scen_name = scenario.scen_name;
	std::ofstream fout("Scenario Data.txt");
	fout << "Scenario data for " << scen_name << ':' << endl << endl;
	fout << "Terrain types for " << scen_name << ':' << endl;
	for(i = 0; i < 256; i++)
		fout << "  Terrain type " << i << ": " << scenario.ter_types[i].name << endl;
	fout << endl << "Monster types for " << scen_name << ':' << endl;
	for(i = 0; i < 256; i++)
		fout << "  Monster type " << i << ": " << scenario.scen_monsters[i].m_name << endl;
	fout << endl << "Item types for " << scen_name << ':' << endl;
	for(i = 0; i < 400; i++)
		fout << "  Item type " << i << ": " << scenario.scen_items[i].full_name << endl;
	fout.close();
}

void scen_text_dump(){
	short i;
	using std::endl;
	location out_sec;
	std::ofstream fout("Scenario Text.txt");
	fout << "Scenario text for " << scenario.scen_name << ':' << endl << endl;
	fout << "Scenario Text:" << endl;
	fout << "Who Wrote 1: " << scenario.who_wrote[0] << endl;
	fout << "Who Wrote 2: " << scenario.who_wrote[1] << endl;
	fout << "Contact Info: " << scenario.contact_info << endl;
	for(i = 0; i < 6; i++)
		if(scenario.intro_strs[i][0] != '*')
			fout << "  Intro Message " << i << ": " << scenario.intro_strs[i] << endl;
	for(i = 0; i < 50; i++)
		if(scenario.journal_strs[i][0] != '*')
			fout << "  Journal Entry " << i << ": " << scenario.journal_strs[i] << endl;
	for(i = 0; i < 50; i++)
		if(scenario.special_items[i].name[0] != '*') {
			fout << "  Special Item " << i << ':' << endl;
			fout << "    Name: " << scenario.special_items[i].name << endl;
			fout << "    Description: " << scenario.special_items[i].descr << endl;
		}
	for(i = 0; i < 100; i++)
		if(scenario.spec_strs[i][0] != '*')
			fout << "  Message " << i << ": " << scenario.spec_strs[i] << endl;
	fout << endl << "Outdoor Sections Text:" << endl << endl;
	for(out_sec.x = 0; out_sec.x < scenario.out_width ; out_sec.x++) {
		for(out_sec.y = 0; out_sec.y < scenario.out_height ; out_sec.y++) {
			fout << "  Section (x = " << (short)out_sec.x << ", y = " << (short)out_sec.y << "):" << endl;
			fout << "    Name: " << scenario.outdoors[out_sec.x][out_sec.y]->out_name;
			fout << "    Comment: " << scenario.outdoors[out_sec.x][out_sec.y]->comment;
			for(i = 0; i < 8; i++)
				if(scenario.outdoors[out_sec.x][out_sec.y]->rect_names[i][0] != '*')
					fout << "    Area Rectangle " << i << ": " << scenario.outdoors[out_sec.x][out_sec.y]->rect_names[i] << endl;
			for(i = 0; i < 90; i++)
				if(scenario.outdoors[out_sec.x][out_sec.y]->spec_strs[i][0] != '*')
					fout << "    Message " << i << ": " << scenario.outdoors[out_sec.x][out_sec.y]->spec_strs[i] << endl;
			for(i = 0; i < 8; i++)
				if(scenario.outdoors[out_sec.x][out_sec.y]->sign_strs[i][0] != '*')
					fout << "    Sign " << i << ": " << scenario.outdoors[out_sec.x][out_sec.y]->sign_strs[i] << endl;
			fout << endl;
		}
	}
	fout << "Town Text:" << endl << endl;
	for(short j = 0; j < scenario.num_towns; j++) {
		fout << "  Town " << j << ':' << endl;
		fout << "  Town Messages:" << endl;
		fout << "    Name: " << scenario.towns[i]->town_name << endl;
		for(i = 0; i < 16; i++)
			if(scenario.towns[i]->rect_names[i][0] != '*')
				fout << "    Area Rectangle " << i << ": " << scenario.towns[i]->rect_names[i] << endl;
		fout << "    Name: " << scenario.towns[i]->town_name << endl;
		for(i = 0; i < 3; i++)
			if(scenario.towns[i]->comment[i][0] != '*')
				fout << "    Comment " << i << ": " << scenario.towns[i]->comment[i] << endl;
		fout << "    Name: " << scenario.towns[i]->town_name << endl;
		for(i = 0; i < 100; i++)
			if(scenario.towns[i]->spec_strs[i][0] != '*')
				fout << "    Message " << i << ": " << scenario.towns[i]->spec_strs[i] << endl;
		fout << "    Name: " << scenario.towns[i]->town_name << endl;
		for(i = 0; i < 20; i++)
			if(scenario.towns[i]->sign_strs[i][0] != '*')
				fout << "    Sign " << i << ": " << scenario.towns[i]->sign_strs[i] << endl;
		fout << endl << "  Town Dialogue:" << endl;
		for(i = 0; i < 10; i++) {
			fout << "    Personality " << j + i << " (" << scenario.towns[i]->talking.people[i].title << "): " << endl;
			fout << "    look: " << scenario.towns[i]->talking.people[i].look << endl;
			fout << "    name: " << scenario.towns[i]->talking.people[i].name << endl;
			fout << "    job: " << scenario.towns[i]->talking.people[i].job << endl;
			fout << "    confused: " << scenario.towns[i]->talking.people[i].dunno << endl;
		}
		for(i = 0; i < 60; i++) {
			if(scenario.towns[i]->talking.talk_nodes[i].str1.length() > 0)
				fout << "    Node " << i << "a: " << scenario.towns[i]->talking.talk_nodes[i].str1 << endl;
			if(scenario.towns[i]->talking.talk_nodes[i].str2.length() > 0)
				fout << "    Node " << i << "b: " << scenario.towns[i]->talking.talk_nodes[i].str2 << endl;
		}
		fout << endl;
	}
	fout.close();
}
