#include <list>
#include <string>
#include "offsets.hpp"
#include <string.h>
#include "functions.hpp"
#include <iostream>
#include "aimbot.cpp"
using namespace uee;

#define debug_cache false

namespace g1
{
	class g2 {
	public:

		static auto actors() -> void { //need to fix this cache system its ass

			temporary_entity_list.clear();
			arrays->uworld = read<uintptr_t>(dynamic_uworld);
			
			
			//for all retards set "#define debug_cache false" to "true"
			
			/*if (debug_cache)
			{
				std::cout << "Uworld : " << arrays->uworld << std::endl;
			}*/

			arrays->game_instance = read<uintptr_t>(arrays->uworld + offset::game_instance);
			arrays->local_player = read<uintptr_t>(read<uintptr_t>(arrays->game_instance + offset::local_player));
			arrays->player_controller = read<uintptr_t>(arrays->local_player + offset::player_controller);
			arrays->acknowledged_pawn = read<uintptr_t>(arrays->player_controller + offset::acknowledged_pawn);
			arrays->skeletal_mesh = read<uintptr_t>(arrays->acknowledged_pawn + offset::skeletal_mesh);
			arrays->player_state = read<uintptr_t>(arrays->acknowledged_pawn + offset::player_state);
			arrays->root_component = read<uintptr_t>(arrays->acknowledged_pawn + offset::root_component);
			arrays->relative_location = read<fvector>(arrays->root_component + offset::relative_location);
			arrays->team_index = read<int>(arrays->player_state + offset::team_index);
			arrays->game_state = read<uintptr_t>(arrays->uworld + offset::game_state);
			arrays->player_array = read<uintptr_t>(arrays->game_state + offset::player_array);
			arrays->player_array_size = read<int>(arrays->game_state + (offset::player_array + sizeof(uintptr_t)));

			for (int i = 0; i < arrays->player_array_size; ++i) {
				auto player_state = read<uintptr_t>(arrays->player_array + (i * sizeof(uintptr_t)));
				auto current_actor = read<uintptr_t>(player_state + offset::pawn_private);

				if (current_actor == arrays->acknowledged_pawn) continue;
				auto skeletalmesh = read<uintptr_t>(current_actor + offset::skeletal_mesh);
				if (!skeletalmesh) continue;

				auto base_bone = ue5->Bone(skeletalmesh, bone::Root);
				if (base_bone.x == 0 && base_bone.y == 0 && base_bone.z == 0) continue;

				if (!ue5->in_screen(ue5->w2s(ue5->Bone(skeletalmesh, bone::Pelvis)))) continue;

				auto is_despawning = (read<char>(current_actor + 0x758) >> 3);

				if (is_despawning) continue;

				auto root_component = read<uintptr_t>(current_actor + offset::root_component);

				auto relative_location1 = read<fvector>(root_component + offset::relative_location);
				auto team_index = read<int>(player_state + offset::team_index);

				if (arrays->acknowledged_pawn)
				{
					auto team_index = read<int>(player_state + offset::team_index);
					if (arrays->team_index == team_index) continue;
				}

				entity cached_actors{ };
				cached_actors.entity = current_actor;
				cached_actors.skeletal_mesh = skeletalmesh;
				cached_actors.root_component = root_component;
				cached_actors.relative_location = relative_location1;
				cached_actors.player_state = player_state;
				cached_actors.team_index = team_index;

				temporary_entity_list.push_back(cached_actors);
			}

			entity_list.clear();
			entity_list = temporary_entity_list;
			std::this_thread::sleep_for(std::chrono::milliseconds(150));

		}
	};
} static g1::g2* g_cache = new g1::g2();
