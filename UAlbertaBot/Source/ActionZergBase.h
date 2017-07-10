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
		//己方
		//单位
		int larva_count;						//幼虫
		int drone_count;						//工蜂
		int zergling_count;						//小狗
		int hydralisk_count;					//刺蛇
		int lurker_count;						//地刺
		int ultralisk_count;					//雷兽
		int defiler_count;						//蝎子
		int overlord_count;						//领主
		int mutalisk_count;						//飞龙
		int scourge_count;						//自爆蚊
		int queen_count;						//女王
		int guardian_count;						//守卫者
		int devourer_count;						//吞噬者

		//建筑
		int hatch_count;						//母巢
		int completed_hatch_count;
		int sunken_count;						//地刺塔
		int spore_count;						//孢子塔

		//军事力量
		double army_supply;
		double air_army_supply;
		double ground_army_supply;

		//判断和倾向
		bool opponent_has_expanded;
		bool can_expand;
		bool force_expand;
		bool being_rushed;
		bool is_attacking;
		bool is_defending;
		bool default_upgrade;

		//敌方
		int enermy_terran_unit_count;
		int enermy_protos_unit_count;
		int enermy_zerg_unit_count;

		//人族单位
		int enermy_marine_count;				//机枪兵
		int enermy_firebat_count;				//火焰兵
		int enermy_medic_count;					//医疗兵
		int enermy_ghost_count;					//幽灵
		int enermy_vulture_count;				//雷车
		int enermy_tank_count;					//坦克
		int enermy_goliath_count;				//机器人
		int enermy_wraith_count;				//隐飞
		int enermy_valkyrie_count;				//导弹护卫舰
		int enermy_bc_count;					//战列巡洋舰
		int enermy_science_vessel_count;		//科学球
		int enermy_dropship_count;				//运输机

		//人族建筑
		int enermy_bunker_count;				//地堡
		int enermy_barrack_count;				//兵营
		int enermy_factory_count;				//工厂
		int enermy_starport_count;				//飞机场

		//神族单位
		int enermy_zealot_count;				//狂热者
		int enermy_dragoon_count;				//龙骑
		int enermy_ht_count;					//光明圣堂
		int enermy_dt_count;					//黑暗圣堂
		int enermy_reaver_count;				//金甲虫
		int enermy_shuttle_count;				//运输机
		int enermy_carrier_count;				//航母
		int enermy_arbiter_count;				//仲裁者
		int enermy_corsair_count;				//海盗船

		//神族建筑
		int enermy_cannon_count;				//光子炮塔
		int enermy_gateway_count;				//兵营
		int enermy_stargate_count;				//星门
		int enermy_robotics_facility_count;		//机械工厂

		//虫族单位
		int enermy_zergling_count;				//小狗
		int enermy_hydralisk_count;				//刺蛇
		int enermy_lurker_count;				//地刺
		int enermy_ultralisk_count;				//雷兽
		int enermy_defiler_count;				//蝎子
		int enermy_mutalisk_count;				//飞龙
		int enermy_queen_count;					//女王

		//虫族建筑
		int enemy_spawning_pool_count;			//孵化池
		int enermy_hydralisk_den_count;			//刺蛇穴
		int enemy_evolution_chamber_count;		//进化腔
		int enermy_spire_count;					//飞龙塔

		//军事力量
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

		//通用建筑和单位
		int enermy_worker_count;
		int enermy_gas_count;
	};
}
