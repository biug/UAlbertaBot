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
		//���������ɵ�λ
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
		
		//���������е�λ
		int larva_in_queue;                      //�������׳�
		int drone_in_queue;                      //�����й���
		int zergling_in_queue;                   //������С��
		int hydralisk_in_queue;                  //�����д���
		int lurker_in_queue;                     //�����еش�
		int ultralisk_in_queue;                  //����������
		int defiler_in_queue;                    //������Ы��
		int overlord_in_queue;                   //����������
		int mutalisk_in_queue;                   //�����з���
		int scourge_in_queue;                    //�������Ա���
		int queen_in_queue;                      //������Ů��
		int guardian_in_queue;                   //������������
		int devourer_in_queue;                    //������������



		//�����ѽ��ɽ���
		int hatchery_count;						//ĸ��
		int extractor_count;                    //����
		int creep_colony_count;                 //ֳ��
		int sunken_colony_count;			    //�ش���
		int spore_colony_count;			    	//������
		int spawning_pool_count;                //Ѫ��
		int hydralisk_den_count;                //���߶�Ѩ
		int lair_count;                         //��Ѩ
		int queens_nest_count;                  //�ʺ�
		int hive_count;                         //�䳲
		int defiler_mound_count;                //Ы�ӳ�
		int spire_count;                        //������
		int greater_spire_count;                //�������
		int nydus_canal_count;                  //Ӽ��ͨ��
		int ultralisk_cavern_count;             //����֮��

		//���������н���
		int hatchery_being_built;						//ĸ��
		int extractor_being_built;                    //����
		int creep_colony_being_built;                 //ֳ��
		int sunken_colony_being_built;			    //�ش���
		int spore_colony_being_built;			   	//������
		int spawning_pool_being_built;                //Ѫ��
		int hydralisk_den_being_built;                //���߶�Ѩ
		int lair_being_built;                         //��Ѩ
		int queens_nest_being_built;                  //�ʺ�
		int hive_being_built;                         //�䳲
		int defiler_mound_being_built;                //Ы�ӳ�
		int spire_being_built;                        //������
		int greater_spire_being_built;                //�������
		int nydus_canal_being_built;                  //Ӽ��ͨ��
		int ultralisk_cavern_being_built;             //����֮��

		//���������н���
		int hatchery_in_queue;						//ĸ��
		int extractor_in_queue;                    //����
		int creep_colony_in_queue;                 //ֳ��
		int sunken_colony_in_queue;			    //�ش���
		int spore_colony_in_queue;			    	//������
		int spawning_pool_in_queue;                //Ѫ��
		int hydralisk_den_in_queue;                //���߶�Ѩ
		int lair_in_queue;                         //��Ѩ
		int queens_nest_in_queue;                  //�ʺ�
		int hive_in_queue;                         //�䳲
		int defiler_mound_in_queue;                //Ы�ӳ�
		int spire_in_queue;                        //������
		int greater_spire_in_queue;                //�������
		int nydus_canal_in_queue;                  //Ӽ��ͨ��
		int ultralisk_cavern_in_queue;             //����֮��



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
