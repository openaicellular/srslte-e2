/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2AP-IEs"
 * 	found in "/nexran/lib/e2ap/messages/e2ap-v02.03.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -fno-include-deps -fincludes-quoted -D /nexran/build/lib/e2ap/E2AP/`
 */

#ifndef	_E2AP_RICindicationType_H_
#define	_E2AP_RICindicationType_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeEnumerated.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum E2AP_RICindicationType {
	E2AP_RICindicationType_report	= 0,
	E2AP_RICindicationType_insert	= 1
	/*
	 * Enumeration is extensible
	 */
} e_E2AP_RICindicationType;

/* E2AP_RICindicationType */
typedef long	 E2AP_RICindicationType_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_E2AP_RICindicationType_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_E2AP_RICindicationType;
extern const asn_INTEGER_specifics_t asn_SPC_E2AP_RICindicationType_specs_1;
asn_struct_free_f E2AP_RICindicationType_free;
asn_struct_print_f E2AP_RICindicationType_print;
asn_constr_check_f E2AP_RICindicationType_constraint;
ber_type_decoder_f E2AP_RICindicationType_decode_ber;
der_type_encoder_f E2AP_RICindicationType_encode_der;
xer_type_decoder_f E2AP_RICindicationType_decode_xer;
xer_type_encoder_f E2AP_RICindicationType_encode_xer;
per_type_decoder_f E2AP_RICindicationType_decode_uper;
per_type_encoder_f E2AP_RICindicationType_encode_uper;
per_type_decoder_f E2AP_RICindicationType_decode_aper;
per_type_encoder_f E2AP_RICindicationType_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _E2AP_RICindicationType_H_ */
#include "asn_internal.h"
