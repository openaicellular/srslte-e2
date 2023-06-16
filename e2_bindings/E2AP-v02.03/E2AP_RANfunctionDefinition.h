/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2AP-IEs"
 * 	found in "/nexran/lib/e2ap/messages/e2ap-v02.03.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -fno-include-deps -fincludes-quoted -D /nexran/build/lib/e2ap/E2AP/`
 */

#ifndef	_E2AP_RANfunctionDefinition_H_
#define	_E2AP_RANfunctionDefinition_H_


#include "asn_application.h"

/* Including external dependencies */
#include "OCTET_STRING.h"

#ifdef __cplusplus
extern "C" {
#endif

/* E2AP_RANfunctionDefinition */
typedef OCTET_STRING_t	 E2AP_RANfunctionDefinition_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_E2AP_RANfunctionDefinition;
asn_struct_free_f E2AP_RANfunctionDefinition_free;
asn_struct_print_f E2AP_RANfunctionDefinition_print;
asn_constr_check_f E2AP_RANfunctionDefinition_constraint;
ber_type_decoder_f E2AP_RANfunctionDefinition_decode_ber;
der_type_encoder_f E2AP_RANfunctionDefinition_encode_der;
xer_type_decoder_f E2AP_RANfunctionDefinition_decode_xer;
xer_type_encoder_f E2AP_RANfunctionDefinition_encode_xer;
per_type_decoder_f E2AP_RANfunctionDefinition_decode_uper;
per_type_encoder_f E2AP_RANfunctionDefinition_encode_uper;
per_type_decoder_f E2AP_RANfunctionDefinition_decode_aper;
per_type_encoder_f E2AP_RANfunctionDefinition_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _E2AP_RANfunctionDefinition_H_ */
#include "asn_internal.h"
