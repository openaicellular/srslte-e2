/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2AP-IEs"
 * 	found in "/nexran/lib/e2ap/messages/e2ap-v02.03.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -fno-include-deps -fincludes-quoted -D /nexran/build/lib/e2ap/E2AP/`
 */

#ifndef	_E2AP_ENB_ID_Choice_H_
#define	_E2AP_ENB_ID_Choice_H_


#include "asn_application.h"

/* Including external dependencies */
#include "BIT_STRING.h"
#include "constr_CHOICE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum E2AP_ENB_ID_Choice_PR {
	E2AP_ENB_ID_Choice_PR_NOTHING,	/* No components present */
	E2AP_ENB_ID_Choice_PR_enb_ID_macro,
	E2AP_ENB_ID_Choice_PR_enb_ID_shortmacro,
	E2AP_ENB_ID_Choice_PR_enb_ID_longmacro
	/* Extensions may appear below */
	
} E2AP_ENB_ID_Choice_PR;

/* E2AP_ENB-ID-Choice */
typedef struct E2AP_ENB_ID_Choice {
	E2AP_ENB_ID_Choice_PR present;
	union E2AP_ENB_ID_Choice_u {
		BIT_STRING_t	 enb_ID_macro;
		BIT_STRING_t	 enb_ID_shortmacro;
		BIT_STRING_t	 enb_ID_longmacro;
		/*
		 * This type is extensible,
		 * possible extensions are below.
		 */
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} E2AP_ENB_ID_Choice_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_E2AP_ENB_ID_Choice;
extern asn_CHOICE_specifics_t asn_SPC_E2AP_ENB_ID_Choice_specs_1;
extern asn_TYPE_member_t asn_MBR_E2AP_ENB_ID_Choice_1[3];
extern asn_per_constraints_t asn_PER_type_E2AP_ENB_ID_Choice_constr_1;

#ifdef __cplusplus
}
#endif

#endif	/* _E2AP_ENB_ID_Choice_H_ */
#include "asn_internal.h"
