#ifndef USER_CORE_INC_FLASH_FLASH_H_
#define USER_CORE_INC_FLASH_FLASH_H_

#include <map>
#include <vector>

#include "stm32h5xx_hal.h"
#include "stm32h5xx_hal_flash.h"
#include "stm32h5xx_hal_flash_ex.h"

static const constexpr uint32_t FLASH_EDATA_SECTOR_SIZE = FLASH_EDATA_SIZE / 2;
static const uint32_t* FLASH_EDATA_BEGIN { (uint32_t*)(FLASH_EDATA_BASE_NS) };
static const uint32_t* FLASH_EDATA_SECTOR_0_ADDRESS { FLASH_EDATA_BEGIN + FLASH_EDATA_SECTOR_SIZE };
static const uint32_t* FLASH_EDATA_SECTOR_1_ADDRESS { FLASH_EDATA_SECTOR_0_ADDRESS + FLASH_EDATA_SECTOR_SIZE };
static const uint32_t* FLASH_EDATA_SECTOR_2_ADDRESS { FLASH_EDATA_SECTOR_1_ADDRESS + FLASH_EDATA_SECTOR_SIZE };
static const uint32_t* FLASH_EDATA_SECTOR_3_ADDRESS { FLASH_EDATA_SECTOR_2_ADDRESS + FLASH_EDATA_SECTOR_SIZE };
static const uint32_t* FLASH_EDATA_SECTOR_4_ADDRESS { FLASH_EDATA_SECTOR_3_ADDRESS + FLASH_EDATA_SECTOR_SIZE };
static const uint32_t* FLASH_EDATA_SECTOR_5_ADDRESS { FLASH_EDATA_SECTOR_4_ADDRESS + FLASH_EDATA_SECTOR_SIZE };
static const uint32_t* FLASH_EDATA_SECTOR_6_ADDRESS { FLASH_EDATA_SECTOR_5_ADDRESS + FLASH_EDATA_SECTOR_SIZE };
static const uint32_t* FLASH_EDATA_SECTOR_7_ADDRESS { FLASH_EDATA_SECTOR_6_ADDRESS + FLASH_EDATA_SECTOR_SIZE };
static const uint32_t* FLASH_EDATA_END { FLASH_EDATA_BEGIN + ((FLASH_EDATA_SECTOR_SIZE * 8) / sizeof(uint32_t)) };
static const constexpr uint32_t FLASH_EDATA_BANK { FLASH_BANK_1 };

static constexpr uint32_t FLASH_ERASE_ERROR_NONE = 0xFFFFFFFFU;

class Flash {
public:
	Flash(FLASH_TypeDef* flashHandle);
	bool write(uint16_t* pDestination, uint16_t* pInput, size_t len);

	enum Banks : const uint32_t {
		Bank1 = FLASH_BANK_1,
		Bank2 = FLASH_BANK_2,
		Both = FLASH_BANK_BOTH
	};
	enum Sectors : const uint32_t {
		Sector0 = FLASH_SECTOR_0,
		Sector1 = FLASH_SECTOR_1,
		Sector2 = FLASH_SECTOR_2,
		Sector3 = FLASH_SECTOR_3,
		Sector4 = FLASH_SECTOR_4,
		Sector5 = FLASH_SECTOR_5,
		Sector6 = FLASH_SECTOR_6,
		Sector7 = FLASH_SECTOR_7,
		Sector8 = FLASH_SECTOR_8,
		Sector9 = FLASH_SECTOR_9,
		Sector10 = FLASH_SECTOR_10,
		Sector11 = FLASH_SECTOR_11,
		Sector12 = FLASH_SECTOR_12,
		Sector13 = FLASH_SECTOR_13,
		Sector14 = FLASH_SECTOR_14,
		Sector15 = FLASH_SECTOR_15,
		Sector16 = FLASH_SECTOR_16,
		Sector17 = FLASH_SECTOR_17,
		Sector18 = FLASH_SECTOR_18,
		Sector19 = FLASH_SECTOR_19,
		Sector20 = FLASH_SECTOR_20,
		Sector21 = FLASH_SECTOR_21,
		Sector22 = FLASH_SECTOR_22,
		Sector23 = FLASH_SECTOR_23,
		Sector24 = FLASH_SECTOR_24,
		Sector25 = FLASH_SECTOR_25,
		Sector26 = FLASH_SECTOR_26,
		Sector27 = FLASH_SECTOR_27,
		Sector28 = FLASH_SECTOR_28,
		Sector29 = FLASH_SECTOR_29,
		Sector30 = FLASH_SECTOR_30,
		Sector31 = FLASH_SECTOR_31,
		Sector32 = FLASH_SECTOR_32,
		Sector33 = FLASH_SECTOR_33,
		Sector34 = FLASH_SECTOR_34,
		Sector35 = FLASH_SECTOR_35,
		Sector36 = FLASH_SECTOR_36,
		Sector37 = FLASH_SECTOR_37,
		Sector38 = FLASH_SECTOR_38,
		Sector39 = FLASH_SECTOR_39,
		Sector40 = FLASH_SECTOR_40,
		Sector41 = FLASH_SECTOR_41,
		Sector42 = FLASH_SECTOR_42,
		Sector43 = FLASH_SECTOR_43,
		Sector44 = FLASH_SECTOR_44,
		Sector45 = FLASH_SECTOR_45,
		Sector46 = FLASH_SECTOR_46,
		Sector47 = FLASH_SECTOR_47,
		Sector48 = FLASH_SECTOR_48,
		Sector49 = FLASH_SECTOR_49,
		Sector50 = FLASH_SECTOR_50,
		Sector51 = FLASH_SECTOR_51,
		Sector52 = FLASH_SECTOR_52,
		Sector53 = FLASH_SECTOR_53,
		Sector54 = FLASH_SECTOR_54,
		Sector55 = FLASH_SECTOR_55,
		Sector56 = FLASH_SECTOR_56,
		Sector57 = FLASH_SECTOR_57,
		Sector58 = FLASH_SECTOR_58,
		Sector59 = FLASH_SECTOR_59,
		Sector60 = FLASH_SECTOR_60,
		Sector61 = FLASH_SECTOR_61,
		Sector62 = FLASH_SECTOR_62,
		Sector63 = FLASH_SECTOR_63,
		Sector64 = FLASH_SECTOR_64,
		Sector65 = FLASH_SECTOR_65,
		Sector66 = FLASH_SECTOR_66,
		Sector67 = FLASH_SECTOR_67,
		Sector68 = FLASH_SECTOR_68,
		Sector69 = FLASH_SECTOR_69,
		Sector70 = FLASH_SECTOR_70,
		Sector71 = FLASH_SECTOR_71,
		Sector72 = FLASH_SECTOR_72,
		Sector73 = FLASH_SECTOR_73,
		Sector74 = FLASH_SECTOR_74,
		Sector75 = FLASH_SECTOR_75,
		Sector76 = FLASH_SECTOR_76,
		Sector77 = FLASH_SECTOR_77,
		Sector78 = FLASH_SECTOR_78,
		Sector79 = FLASH_SECTOR_79,
		Sector80 = FLASH_SECTOR_80,
		Sector81 = FLASH_SECTOR_81,
		Sector82 = FLASH_SECTOR_82,
		Sector83 = FLASH_SECTOR_83,
		Sector84 = FLASH_SECTOR_84,
		Sector85 = FLASH_SECTOR_85,
		Sector86 = FLASH_SECTOR_86,
		Sector87 = FLASH_SECTOR_87,
		Sector88 = FLASH_SECTOR_88,
		Sector89 = FLASH_SECTOR_89,
		Sector90 = FLASH_SECTOR_90,
		Sector91 = FLASH_SECTOR_91,
		Sector92 = FLASH_SECTOR_92,
		Sector93 = FLASH_SECTOR_93,
		Sector94 = FLASH_SECTOR_94,
		Sector95 = FLASH_SECTOR_95,
		Sector96 = FLASH_SECTOR_96,
		Sector97 = FLASH_SECTOR_97,
		Sector98 = FLASH_SECTOR_98,
		Sector99 = FLASH_SECTOR_99,
		Sector100 = FLASH_SECTOR_100,
		Sector101 = FLASH_SECTOR_101,
		Sector102 = FLASH_SECTOR_102,
		Sector103 = FLASH_SECTOR_103,
		Sector104 = FLASH_SECTOR_104,
		Sector105 = FLASH_SECTOR_105,
		Sector106 = FLASH_SECTOR_106,
		Sector107 = FLASH_SECTOR_107,
		Sector108 = FLASH_SECTOR_108,
		Sector109 = FLASH_SECTOR_109,
		Sector110 = FLASH_SECTOR_110,
		Sector111 = FLASH_SECTOR_111,
		Sector112 = FLASH_SECTOR_112,
		Sector113 = FLASH_SECTOR_113,
		Sector114 = FLASH_SECTOR_114,
		Sector115 = FLASH_SECTOR_115,
		Sector116 = FLASH_SECTOR_116,
		Sector117 = FLASH_SECTOR_117,
		Sector118 = FLASH_SECTOR_118,
		Sector119 = FLASH_SECTOR_119,
		Sector120 = FLASH_SECTOR_120,
		Sector121 = FLASH_SECTOR_121,
		Sector122 = FLASH_SECTOR_122,
		Sector123 = FLASH_SECTOR_123,
		Sector124 = FLASH_SECTOR_124,
		Sector125 = FLASH_SECTOR_125,
		Sector126 = FLASH_SECTOR_126,
		Sector127 = FLASH_SECTOR_127,
	};

protected:
private:
	bool addressRangeInFlash(uint8_t* pDestination, size_t len);
	std::vector<uint32_t*> getSectorsInRange(uint8_t* pDestination, size_t len);
	uint32_t* findSectorForDestination(uint8_t* pDestination);

	struct EraseOptions {
		const Banks bank;
		const Sectors sector;
	};

	bool erase(std::vector<EraseOptions> opts);
	void lock();
	void unlock();

	FLASH_TypeDef* handle;
};


static const std::vector<const uint32_t*> FLASH_EDATA_SECTORS = {
	FLASH_EDATA_SECTOR_0_ADDRESS,
	FLASH_EDATA_SECTOR_1_ADDRESS,
	FLASH_EDATA_SECTOR_2_ADDRESS,
	FLASH_EDATA_SECTOR_3_ADDRESS,
	FLASH_EDATA_SECTOR_4_ADDRESS,
	FLASH_EDATA_SECTOR_5_ADDRESS,
	FLASH_EDATA_SECTOR_6_ADDRESS,
	FLASH_EDATA_SECTOR_7_ADDRESS,
};

static const std::map<const uint32_t*, Flash::Sectors> FLASH_EDATA_SECTOR_MAP = {
	{ FLASH_EDATA_SECTOR_0_ADDRESS , Flash::Sectors::Sector120 },
	{ FLASH_EDATA_SECTOR_1_ADDRESS , Flash::Sectors::Sector121 },
	{ FLASH_EDATA_SECTOR_2_ADDRESS , Flash::Sectors::Sector122 },
	{ FLASH_EDATA_SECTOR_3_ADDRESS , Flash::Sectors::Sector123 },
	{ FLASH_EDATA_SECTOR_4_ADDRESS , Flash::Sectors::Sector124 },
	{ FLASH_EDATA_SECTOR_5_ADDRESS , Flash::Sectors::Sector125 },
	{ FLASH_EDATA_SECTOR_6_ADDRESS , Flash::Sectors::Sector126 },
	{ FLASH_EDATA_SECTOR_7_ADDRESS , Flash::Sectors::Sector127 },
};


#endif /* USER_CORE_INC_FLASH_FLASH_H_ */
