#pragma once
#include "BuildOrder.h"

namespace CasiaBot
{
	class ActionZergBase
	{
	public:
		ActionZergBase();
		~ActionZergBase();
		void updateCurrentState();
		virtual bool canDeployAction() = 0;
		virtual void init() = 0;
		virtual bool tick() = 0;
		virtual void getBuildOrderList(UAlbertaBot::ProductionQueue &queue) = 0;

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
		int enermy_terran_unit_count;
		int enermy_protos_unit_count;
		int enermy_zerg_unit_count;

		//���嵥λ
		int enermy_marine_count;				//��ǹ��
		int enermy_firebat_count;				//�����
		int enermy_medic_count;					//ҽ�Ʊ�
		int enermy_ghost_count;					//����
		int enermy_vulture_count;				//�׳�
		int enermy_tank_count;					//̹��
		int enermy_goliath_count;				//������
		int enermy_wraith_count;				//����
		int enermy_valkyrie_count;				//����������
		int enermy_bc_count;					//ս��Ѳ��
		int enermy_science_vessel_count;		//��ѧ��
		int enermy_dropship_count;				//�����

		//���彨��
		int enermy_bunker_count;				//�ر�
		int enermy_barrack_count;				//��Ӫ
		int enermy_factory_count;				//����
		int enermy_starport_count;				//�ɻ���

		//���嵥λ
		int enermy_zealot_count;				//������
		int enermy_dragoon_count;				//����
		int enermy_ht_count;					//����ʥ��
		int enermy_dt_count;					//�ڰ�ʥ��
		int enermy_reaver_count;				//��׳�
		int enermy_shuttle_count;				//�����
		int enermy_carrier_count;				//��ĸ
		int enermy_arbiter_count;				//�ٲ���
		int enermy_corsair_count;				//������

		//���彨��
		int enermy_cannon_count;				//��������
		int enermy_gateway_count;				//��Ӫ
		int enermy_stargate_count;				//����
		int enermy_robotics_facility_count;		//��е����

		//���嵥λ
		int enermy_zergling_count;				//С��
		int enermy_hydralisk_count;				//����
		int enermy_lurker_count;				//�ش�
		int enermy_ultralisk_count;				//����
		int enermy_defiler_count;				//Ы��
		int enermy_mutalisk_count;				//����
		int enermy_queen_count;					//Ů��

		//���彨��
		int enemy_spawning_pool_count;			//������
		int enermy_hydralisk_den_count;			//����Ѩ
		int enemy_evolution_chamber_count;		//����ǻ
		int enermy_spire_count;					//������

		//��������
		double enermy_army_supply;
		double enermy_air_army_supply;
		double enermy_ground_army_supply;
		double enermy_ground_large_army_supply;
		double enermy_ground_small_army_supply;
		double enemy_anti_air_army_supply;
		double enemy_biological_army_supply;
		int enemy_static_defence_count;
		int enemy_proxy_building_count;
		double enemy_attacking_army_supply;
		int enemy_attacking_worker_count;
		int enemy_cloaked_unit_count;

		//ͨ�ý����͵�λ
		int enermy_worker_count;
		int enermy_gas_count;
	};
}
