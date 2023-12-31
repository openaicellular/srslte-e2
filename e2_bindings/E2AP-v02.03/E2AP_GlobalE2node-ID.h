/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2AP-IEs"
 * 	found in "/nexran/lib/e2ap/messages/e2ap-v02.03.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -fno-include-deps -fincludes-quoted -D /nexran/build/lib/e2ap/E2AP/`
 */

#ifndef	_E2AP_GlobalE2node_ID_H_
#define	_E2AP_GlobalE2node_ID_H_


#include "asn_application.h"

/* Including external dependencies */
#include "E2AP_GlobalE2node-gNB-ID.h"
#include "E2AP_GlobalE2node-en-gNB-ID.h"
#include "E2AP_GlobalE2node-ng-eNB-ID.h"
#include "E2AP_GlobalE2node-eNB-ID.h"
#include "constr_CHOICE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum E2AP_GlobalE2node_ID_PR {
	E2AP_GlobalE2node_ID_PR_NOTHING,	/* No components present */
	E2AP_GlobalE2node_ID_PR_gNB,
	E2AP_GlobalE2node_ID_PR_en_gNB,
	E2AP_GlobalE2node_ID_PR_ng_eNB,
	E2AP_GlobalE2node_ID_PR_eNB
	/* Extensions may appear below */
	
} E2AP_GlobalE2node_ID_PR;

/* E2AP_GlobalE2node-ID */
typedef struct E2AP_GlobalE2node_ID {
	E2AP_GlobalE2node_ID_PR present;
	union E2AP_GlobalE2node_ID_u {
		E2AP_GlobalE2node_gNB_ID_t	 gNB;
		E2AP_GlobalE2node_en_gNB_ID_t	 en_gNB;
		E2AP_GlobalE2node_ng_eNB_ID_t	 ng_eNB;
		E2AP_GlobalE2node_eNB_ID_t	 eNB;
		/*
		 * This type is extensible,
		 * possible extensions are below.
		 */
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} E2AP_GlobalE2node_ID_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_E2AP_GlobalE2node_ID;
extern asn_CHOICE_specifics_t asn_SPC_E2AP_GlobalE2node_ID_specs_1;
extern asn_TYPE_member_t asn_MBR_E2AP_GlobalE2node_ID_1[4];
extern asn_per_constraints_t asn_PER_type_E2AP_GlobalE2node_ID_constr_1;

#ifdef __cplusplus
}
#endif

#endif	/* _E2AP_GlobalE2node_ID_H_ */
#include "asn_internal.h"
