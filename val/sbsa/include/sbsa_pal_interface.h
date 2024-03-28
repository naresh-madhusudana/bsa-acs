/** @file
 * Copyright (c) 2024, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

#ifndef __SBSA_PAL_INTERFACE_H__
#define __SBSA_PAL_INTERFACE_H__

#include "../../common/include/pal_interface.h"

#if defined(TARGET_LINUX) || defined(TARGET_EMULATION)
uint32_t pal_get_device_path(const char *hid, char hid_path[][MAX_NAMED_COMP_LENGTH]);
uint32_t pal_smmu_is_etr_behind_catu(char *etr_path);
#endif

/* PCIe related PAL APIs */
uint32_t pal_pcie_get_rp_transaction_frwd_support(uint32_t seg, uint32_t bus,
                                                            uint32_t dev, uint32_t fn);

/* Exerciser related PAL APIs */
void     pal_exerciser_disable_rp_pio_register(uint32_t bdf);
uint32_t pal_exerciser_check_poison_data_forwarding_support(void);
uint32_t pal_exerciser_get_pcie_ras_compliant_err_node(uint32_t bdf, uint32_t rp_bdf);
uint64_t pal_exerciser_get_ras_status(uint32_t ras_node, uint32_t e_bdf, uint32_t erp_bdf);
uint32_t pal_exerciser_set_bar_response(uint32_t bdf);

/* NIST related APIs */
uint32_t pal_nist_generate_rng(uint32_t *rng_buffer);

/* PMU related APIs and structures*/

/**
  @brief  Instance of system pmu info
**/
typedef struct {
    uint8_t  type;                /* The component that this PMU block is associated with*/
    uint64_t primary_instance;    /* Primary node instance, specific to the PMU type*/
    uint32_t secondary_instance;  /* Secondary node instance, specific to the PMU type*/
    uint8_t  dual_page_extension; /* Support of the dual-page mode*/
    uint64_t base0;               /* Base address of Page 0 of the PMU*/
    uint64_t base1;               /* Base address of Page 1 of the PMU,
                                   valid only if dual_page_extension is 1*/
} PMU_INFO_BLOCK;

typedef struct {
    uint32_t pmu_count;          /* Total number of PMU info blocks*/
    PMU_INFO_BLOCK  info[];      /* PMU info blocks for each PMU nodes*/
} PMU_INFO_TABLE;

/**
  @brief  SRAT node type
**/

typedef enum {
  SRAT_NODE_MEM_AFF  = 0x01,
  SRAT_NODE_GICC_AFF = 0x03
} SRAT_NODE_TYPE_e;

/**
  @brief  SRAT GICC Affinity Structure
**/

typedef struct {
  uint32_t   prox_domain;      /* Proximity domain*/
  uint32_t   proc_uid;         /* ACPI Processor UID */
  uint32_t   flags;            /* Flags*/
  uint32_t   clk_domain;       /* Clock Domain*/
} SRAT_GICC_AFF_ENTRY;

/**
  @brief  SRAT Memory Affinity Structure
**/

typedef struct {
  uint32_t   prox_domain;     /* Proximity domain */
  uint32_t   flags;           /* flags */
  uint64_t   addr_base;       /* mem range address base */
  uint64_t   addr_len;        /* mem range address len */
} SRAT_MEM_AFF_ENTRY;

typedef union {
  SRAT_MEM_AFF_ENTRY mem_aff;
  SRAT_GICC_AFF_ENTRY gicc_aff;
} SRAT_NODE_INFO;

typedef struct {
  uint32_t node_type;         /* Node type*/
  SRAT_NODE_INFO node_data;
} SRAT_INFO_ENTRY;

typedef struct {
  uint32_t num_of_srat_entries;
  uint32_t num_of_mem_ranges;
  SRAT_INFO_ENTRY  srat_info[];
} SRAT_INFO_TABLE;

typedef enum {
  PMU_EVENT_IB_TOTAL_BW,        /* Inbound total bandwidth     */
  PMU_EVENT_OB_TOTAL_BW,        /* Outbound total bandwidth    */
  PMU_EVENT_IB_READ_BW,         /* Inbound read bandwidth      */
  PMU_EVENT_IB_WRITE_BW,        /* Inbound write bandwidth     */
  PMU_EVENT_OB_READ_BW,         /* Outbound read bandwidth     */
  PMU_EVENT_OB_WRITE_BW,        /* Outbound write bandwidth    */
  PMU_EVENT_IB_OPEN_TXN,        /* Inbound open transactions   */
  PMU_EVENT_IB_TOTAL_TXN,       /* Inbound total transactions  */
  PMU_EVENT_OB_OPEN_TXN,        /* Outbound open transactions  */
  PMU_EVENT_OB_TOTAL_TXN,       /* Outbound total transactions */
  PMU_EVENT_LOCAL_BW,           /* Local traffic bandwidth     */
  PMU_EVENT_REMOTE_BW,          /* Remote trafic bandwidth     */
  PMU_EVENT_ALL_BW,             /* All traffic bandwidth       */
  PMU_EVENT_TRAFFIC_1,          /* traffic type 1 */
  PMU_EVENT_TRAFFIC_2           /* traffic type 2 */
} PMU_EVENT_TYPE_e;

/* PMU node types */
typedef enum {
  PMU_NODE_MEM_CNTR,
  PMU_NODE_SMMU,
  PMU_NODE_PCIE_RC,
  PMU_NODE_ACPI_DEVICE,
  PMU_NODE_PE_CACHE
} PMU_NODE_INFO_TYPE;


#define PMU_EVENT_INVALID 0xFFFFFFFF

/**
  @brief  This API fills in the PMU_INFO_TABLE with information about local and system
          timers in the system. This is achieved by parsing the ACPI - APMT table.

  @param  PmuTable  - Address where the PMU information needs to be filled.

  @return  None
**/
void pal_pmu_create_info_table(PMU_INFO_TABLE *PmuTable);
uint32_t pal_pmu_get_event_info(PMU_EVENT_TYPE_e event_type, PMU_NODE_INFO_TYPE node_type);
uint32_t pal_pmu_get_multi_traffic_support_interface(uint64_t *interface_acpiid,
                                                       uint32_t *num_traffic_type_support);
uint32_t pal_generate_traffic(uint64_t interface_acpiid, uint32_t pmu_node_index,
                                     uint32_t mon_index, uint32_t eventid);
uint32_t pal_pmu_check_monitor_count_value(uint64_t interface_acpiid, uint32_t count_value,
                                                               uint32_t eventid);

/* Cache info table structures and APIs */

#define CACHE_TYPE_SHARED  0x0
#define CACHE_TYPE_PRIVATE 0x1
#define CACHE_INVALID_NEXT_LVL_IDX 0xFFFFFFFF
#define CACHE_INVALID_IDX 0xFFFFFFFF

/*only the fields and flags required by ACS are parsed from ACPI PPTT table*/
/*Cache flags indicate validity of cache info provided by PPTT Table*/
typedef struct {
  uint32_t size_property_valid;
  uint32_t cache_type_valid;
  uint32_t cache_id_valid;
} CACHE_FLAGS;

/* Since most of platform doesn't support cache id field (ACPI 6.4+), ACS uses PPTT offset as key
   to uniquely identify a cache, In future once platforms align with ACPI 6.4+ my_offset member
   might be removed from cache entry*/
typedef struct {
  CACHE_FLAGS flags;          /* Cache flags */
  uint32_t my_offset;         /* Cache PPTT structure offset */
  uint32_t next_level_index;  /* Index of next level cache entry in CACHE_INFO_TABLE */
  uint32_t size;              /* Size of the cache in bytes */
  uint32_t cache_id;          /* Unique, non-zero identifier for this cache */
  uint32_t is_private;        /* Field indicate whether cache is private */
  uint8_t  cache_type;        /* Cache type */
} CACHE_INFO_ENTRY;

typedef struct {
  uint32_t num_of_cache;            /* Total of number of cache info entries */
  CACHE_INFO_ENTRY cache_info[];    /* Array of cache info entries */
} CACHE_INFO_TABLE;

void pal_cache_create_info_table(CACHE_INFO_TABLE *CacheTable, PE_INFO_TABLE *PeTable);
void pal_cache_dump_info_table(CACHE_INFO_TABLE *CacheTable, PE_INFO_TABLE *PeTable);

/*
 * @brief MPAM Resource Node
 */

#define MPAM_NEXT_MSC(msc_entry) \
        (MPAM_MSC_NODE *)((uint8_t *)(&msc_entry->rsrc_node[0]) \
        + msc_entry->rsrc_count * sizeof(MPAM_RESOURCE_NODE))

typedef struct {
    uint8_t    ris_index;
    uint8_t    locator_type;  /* Identifies location of this resource */
    uint64_t   descriptor1;   /* Primary acpi description of location */
    uint32_t   descriptor2;   /* Secondary acpi description of location */
} MPAM_RESOURCE_NODE;

/*
 * @brief MPAM MSC Node
 */
typedef struct {
    uint64_t           msc_base_addr; /* base addr of mem-map MSC reg */
    uint32_t           msc_addr_len;  /*  MSC mem map size */
    uint32_t           max_nrdy;      /* max time in microseconds that MSC not ready
                                         after config change */
    uint32_t           rsrc_count;    /* number of resource nodes */
    MPAM_RESOURCE_NODE rsrc_node[]; /* Details of resource node */
} MPAM_MSC_NODE;

/*
 * @brief Mpam info table
 */
typedef struct {
    uint32_t          msc_count;  /* Number of MSC node */
    MPAM_MSC_NODE     msc_node[]; /* Details of MSC node */
} MPAM_INFO_TABLE;

void pal_mpam_create_info_table(MPAM_INFO_TABLE *MpamTable);
void *pal_mem_alloc_at_address(uint64_t mem_base, uint64_t size);
void pal_mem_free_at_address(uint64_t mem_base, uint64_t size);

/* RAS INFO table */

typedef enum {
  NODE_TYPE_PE = 0x0,
  NODE_TYPE_MC = 0x1,
  NODE_TYPE_SMMU = 0x2,
  NODE_TYPE_VDR = 0x3,
  NODE_TYPE_GIC = 0x4,
  NODE_TYPE_LAST_ENTRY
} RAS_NODE_TYPE_e;

typedef enum {
  RAS_INTF_TYPE_SYS_REG,   /* System register RAS node interface type */
  RAS_INTF_TYPE_MMIO       /* MMIO RAS node interface type */
} RAS_NODE_INTF_TYPE;

typedef struct {
  uint32_t  processor_id;
  uint32_t  resource_type;
  uint32_t  flags;
  uint64_t  affinity;
  uint64_t  res_specific_data;  /* Resource Specific Data */
} RAS_NODE_PE_DATA;

typedef struct {
  uint32_t  proximity_domain;
} RAS_NODE_MC_DATA;

typedef struct {
  RAS_NODE_INTF_TYPE  intf_type;           /* Interface Type */
  uint32_t  flags;
  uint64_t  base_addr;
  uint32_t  start_rec_index;     /* Start Record Index */
  uint32_t  num_err_rec;
  uint64_t  err_rec_implement;
  uint64_t  err_status_reporting;
  uint64_t  addressing_mode;
} RAS_INTERFACE_INFO;

typedef struct {
  uint32_t  type;
  uint32_t  flag;
  uint32_t  gsiv;
  uint32_t  its_grp_id;
} RAS_INTERRUPT_INFO;

typedef union {
  RAS_NODE_PE_DATA    pe;
  RAS_NODE_MC_DATA    mc;
} RAS_NODE_DATA;

typedef struct {
  RAS_NODE_TYPE_e     type;              /* Node Type PE/GIC/SMMU */
  uint16_t            length;            /* Length of the Node */
  uint64_t            num_intr_entries;  /* Number of Interrupt Entry */
  RAS_NODE_DATA       node_data;         /* Node Specific Data */
  RAS_INTERFACE_INFO  intf_info;         /* Node Interface Info */
  RAS_INTERRUPT_INFO  intr_info[2];      /* Node Interrupt Info */
} RAS_NODE_INFO;

typedef struct {
  uint32_t  num_nodes;    /* Number of total RAS Nodes */
  uint32_t  num_pe_node;  /* Number of PE RAS Nodes */
  uint32_t  num_mc_node;  /* Number of Memory Controller Nodes */
  RAS_NODE_INFO  node[];  /* Array of RAS nodes */
} RAS_INFO_TABLE;

typedef enum {
    ERR_UC = 0x1,    /* UnContainable Error */
    ERR_DE,          /* Deferred Error */
    ERR_CE,          /* Correctable Error */
    ERR_CRITICAL,    /* Critical Error */
    ERR_CONTAINABLE  /* Containable Error */
} RAS_ERROR_TYPE;

typedef struct {
   RAS_ERROR_TYPE ras_error_type;   /* Error Type */
   uint64_t error_pa;                 /* Error Phy Address */
   uint32_t rec_index;                /* Error Record Index */
   uint32_t node_index;               /* Error Node Index in Info table */
   uint8_t is_pfg_check;              /* Pseudo Fault Check or not */
} RAS_ERR_IN_t;

typedef struct {
   uint32_t intr_id;        /* Interrupt ID */
   uint32_t error_record;   /* Error Record Number */
} RAS_ERR_OUT_t;

typedef enum {
  RAS2_TYPE_MEMORY = 0   /* RAS2 memory feature type*/
} RAS2_FEAT_TYPE;

typedef struct {
  uint32_t  proximity_domain;        /* Proximity domain of the memory */
  uint32_t  patrol_scrub_support;    /* Patrol srub support flag */
} RAS2_MEM_INFO;

typedef union {
  RAS2_MEM_INFO mem_feat_info;       /* Memory feature specific info */
} RAS2_BLOCK_INFO;

typedef struct {
  RAS2_FEAT_TYPE type;                      /* RAS2 feature type*/
  RAS2_BLOCK_INFO block_info;        /* RAS2 block info */
} RAS2_BLOCK;

typedef struct {
  uint32_t num_all_block;       /* Number of RAS2 feature blocks */
  uint32_t num_of_mem_block;    /* Number of memory feature blocks */
  RAS2_BLOCK blocks[];
} RAS2_INFO_TABLE;

void pal_ras2_create_info_table(RAS2_INFO_TABLE *ras2_info_table);
void pal_ras_create_info_table(RAS_INFO_TABLE *ras_info_table);

uint32_t pal_ras_setup_error(RAS_ERR_IN_t in_param, RAS_ERR_OUT_t *out_param);
uint32_t pal_ras_inject_error(RAS_ERR_IN_t in_param, RAS_ERR_OUT_t *out_param);
void pal_ras_wait_timeout(uint32_t count);
uint32_t pal_ras_check_plat_poison_support(void);

typedef struct {
  uint32_t mem_prox_domain;             /* Proximity domain of the memory region*/
  uint64_t write_bw;                    /* Maximum write bandwidth */
  uint64_t read_bw;                     /* Maximum read bandwidth */
} HMAT_BW_ENTRY;

typedef struct {
  uint32_t num_of_mem_prox_domain;      /* Number of Memory Proximity Domains */
  HMAT_BW_ENTRY bw_info[];            /* Array of bandwidth info based on proximity domain */
} HMAT_INFO_TABLE;

void pal_hmat_create_info_table(HMAT_INFO_TABLE *HmatTable);
void pal_srat_create_info_table(SRAT_INFO_TABLE *SratTable);

#endif
