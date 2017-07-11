#pragma once
#include "InformationManager.h"
#include "ProductionQueue.h"
#include "BuildOrder.h"

namespace CasiaBot
{
	class ActionZergBase
	{
	public:
		ActionZergBase();
		~ActionZergBase();
		virtual bool canDeployAction() = 0;
		virtual void init() = 0;
		virtual bool tick() = 0;
		virtual void getBuildOrderList(UAlbertaBot::ProductionQueue &queue) = 0;
		virtual void updateCurrentState(UAlbertaBot::ProductionQueue &queue);

	protected:
		std::pair<BWAPI::TilePosition, BWAPI::TilePosition> getClosestOpponentBaseLocation();
		double unitPathingDistance(BWAPI::UnitType type, std::pair<BWAPI::TilePosition, BWAPI::TilePosition> fromto);

	protected:
		//����
		//��λ
		int larva_count;						//�׳�
		int drone_count;						//����
		int zergling_count;						//С��
		int hydralisk_count;					//����
		int lurker_count;						//�ش�
		int ultralisk_count;					//����
		int defiler_count;						//Ы��
		int overlord_count;						//����
		int mutalisk_count;						//����
		int scourge_count;						//�Ա���
		int queen_count;						//Ů��
		int guardian_count;						//������
		int devourer_count;						//������

												//����
		int hatch_count;						//ĸ��
		int completed_hatch_count;
		int sunken_count;						//�ش���
		int spore_count;						//������
		int spawning_pool_count;
		int hydralisk_den_count;
		int queens_nest_count;
		int lair_count;
		int hive_count;
		int extractor_count;
		int defiler_mount_count;
		int spire_count;

												//��������
		double army_supply;
		double air_army_supply;
		double ground_army_supply;

		//�жϺ�����
		bool opponent_has_expanded;
		bool can_expand;
		bool force_expand;
		bool being_rushed;
		bool is_attacking;
		bool is_defending;
		bool default_upgrade;

		//�з�
		int enemy_terran_unit_count;
		int enemy_protos_unit_count;
		int enemy_zerg_unit_count;

		//���嵥λ
		int enemy_marine_count;				//��ǹ��
		int enemy_firebat_count;				//�����
		int enemy_medic_count;					//ҽ�Ʊ�
		int enemy_ghost_count;					//����
		int enemy_vulture_count;				//�׳�
		int enemy_tank_count;					//̹��
		int enemy_goliath_count;				//������
		int enemy_wraith_count;				//����
		int enemy_valkyrie_count;				//����������
		int enemy_bc_count;					//ս��Ѳ��
		int enemy_science_vessel_count;		//��ѧ��
		int enemy_dropship_count;				//�����

												//���彨��
		int enemy_bunker_count;				//�ر�
		int enemy_barrack_count;				//��Ӫ
		int enemy_factory_count;				//����
		int enemy_starport_count;				//�ɻ���

												//���嵥λ
		int enemy_zealot_count;				//������
		int enemy_dragoon_count;				//����
		int enemy_ht_count;					//����ʥ��
		int enemy_dt_count;					//�ڰ�ʥ��
		int enemy_reaver_count;				//��׳�
		int enemy_shuttle_count;				//�����
		int enemy_carrier_count;				//��ĸ
		int enemy_arbiter_count;				//�ٲ���
		int enemy_corsair_count;				//������

												//���彨��
		int enemy_cannon_count;				//��������
		int enemy_gateway_count;				//��Ӫ
		int enemy_stargate_count;				//����
		int enemy_robotics_facility_count;		//��е����

												//���嵥λ
		int enemy_zergling_count;				//С��
		int enemy_hydralisk_count;				//����
		int enemy_lurker_count;				//�ش�
		int enemy_ultralisk_count;				//����
		int enemy_defiler_count;				//Ы��
		int enemy_mutalisk_count;				//����
		int enemy_queen_count;					//Ů��

												//���彨��
		int enemy_spawning_pool_count;			//������
		int enemy_hydralisk_den_count;			//����Ѩ
		int enemy_evolution_chamber_count;		//����ǻ
		int enemy_spire_count;					//������

												//��������
		double enemy_army_supply;
		double enemy_air_army_supply;
		double enemy_ground_army_supply;
		double enemy_ground_large_army_supply;
		double enemy_ground_small_army_supply;
		double enemy_anti_air_army_supply;
		double enemy_biological_army_supply;
		int enemy_static_defence_count;
		int enemy_proxy_building_count;
		double enemy_attacking_army_supply;
		int enemy_attacking_worker_count;
		int enemy_cloaked_unit_count;

		//ͨ�ý����͵�λ
		int enemy_worker_count;
		int enemy_gas_count;
	};
}
