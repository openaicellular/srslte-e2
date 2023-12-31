/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2AP-CommonDataTypes"
 * 	found in "/nexran/lib/e2ap/messages/e2ap-v02.03.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -fno-include-deps -fincludes-quoted -D /nexran/build/lib/e2ap/E2AP/`
 */

#ifndef	_E2AP_ProtocolIE_ID_H_
#define	_E2AP_ProtocolIE_ID_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeInteger.h"

#ifdef __cplusplus
extern "C" {
#endif

/* E2AP_ProtocolIE-ID */
typedef long	 E2AP_ProtocolIE_ID_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_E2AP_ProtocolIE_ID_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_E2AP_ProtocolIE_ID;
asn_struct_free_f E2AP_ProtocolIE_ID_free;
asn_struct_print_f E2AP_ProtocolIE_ID_print;
asn_constr_check_f E2AP_ProtocolIE_ID_constraint;
ber_type_decoder_f E2AP_ProtocolIE_ID_decode_ber;
der_type_encoder_f E2AP_ProtocolIE_ID_encode_der;
xer_type_decoder_f E2AP_ProtocolIE_ID_decode_xer;
xer_type_encoder_f E2AP_ProtocolIE_ID_encode_xer;
per_type_decoder_f E2AP_ProtocolIE_ID_decode_uper;
per_type_encoder_f E2AP_ProtocolIE_ID_encode_uper;
per_type_decoder_f E2AP_ProtocolIE_ID_decode_aper;
per_type_encoder_f E2AP_ProtocolIE_ID_encode_aper;
#define E2AP_ProtocolIE_ID_id_Cause	((E2AP_ProtocolIE_ID_t)1)
#define E2AP_ProtocolIE_ID_id_CriticalityDiagnostics	((E2AP_ProtocolIE_ID_t)2)
#define E2AP_ProtocolIE_ID_id_GlobalE2node_ID	((E2AP_ProtocolIE_ID_t)3)
#define E2AP_ProtocolIE_ID_id_GlobalRIC_ID	((E2AP_ProtocolIE_ID_t)4)
#define E2AP_ProtocolIE_ID_id_RANfunctionID	((E2AP_ProtocolIE_ID_t)5)
#define E2AP_ProtocolIE_ID_id_RANfunctionID_Item	((E2AP_ProtocolIE_ID_t)6)
#define E2AP_ProtocolIE_ID_id_RANfunctionIEcause_Item	((E2AP_ProtocolIE_ID_t)7)
#define E2AP_ProtocolIE_ID_id_RANfunction_Item	((E2AP_ProtocolIE_ID_t)8)
#define E2AP_ProtocolIE_ID_id_RANfunctionsAccepted	((E2AP_ProtocolIE_ID_t)9)
#define E2AP_ProtocolIE_ID_id_RANfunctionsAdded	((E2AP_ProtocolIE_ID_t)10)
#define E2AP_ProtocolIE_ID_id_RANfunctionsDeleted	((E2AP_ProtocolIE_ID_t)11)
#define E2AP_ProtocolIE_ID_id_RANfunctionsModified	((E2AP_ProtocolIE_ID_t)12)
#define E2AP_ProtocolIE_ID_id_RANfunctionsRejected	((E2AP_ProtocolIE_ID_t)13)
#define E2AP_ProtocolIE_ID_id_RICaction_Admitted_Item	((E2AP_ProtocolIE_ID_t)14)
#define E2AP_ProtocolIE_ID_id_RICactionID	((E2AP_ProtocolIE_ID_t)15)
#define E2AP_ProtocolIE_ID_id_RICaction_NotAdmitted_Item	((E2AP_ProtocolIE_ID_t)16)
#define E2AP_ProtocolIE_ID_id_RICactions_Admitted	((E2AP_ProtocolIE_ID_t)17)
#define E2AP_ProtocolIE_ID_id_RICactions_NotAdmitted	((E2AP_ProtocolIE_ID_t)18)
#define E2AP_ProtocolIE_ID_id_RICaction_ToBeSetup_Item	((E2AP_ProtocolIE_ID_t)19)
#define E2AP_ProtocolIE_ID_id_RICcallProcessID	((E2AP_ProtocolIE_ID_t)20)
#define E2AP_ProtocolIE_ID_id_RICcontrolAckRequest	((E2AP_ProtocolIE_ID_t)21)
#define E2AP_ProtocolIE_ID_id_RICcontrolHeader	((E2AP_ProtocolIE_ID_t)22)
#define E2AP_ProtocolIE_ID_id_RICcontrolMessage	((E2AP_ProtocolIE_ID_t)23)
#define E2AP_ProtocolIE_ID_id_RICcontrolStatus	((E2AP_ProtocolIE_ID_t)24)
#define E2AP_ProtocolIE_ID_id_RICindicationHeader	((E2AP_ProtocolIE_ID_t)25)
#define E2AP_ProtocolIE_ID_id_RICindicationMessage	((E2AP_ProtocolIE_ID_t)26)
#define E2AP_ProtocolIE_ID_id_RICindicationSN	((E2AP_ProtocolIE_ID_t)27)
#define E2AP_ProtocolIE_ID_id_RICindicationType	((E2AP_ProtocolIE_ID_t)28)
#define E2AP_ProtocolIE_ID_id_RICrequestID	((E2AP_ProtocolIE_ID_t)29)
#define E2AP_ProtocolIE_ID_id_RICsubscriptionDetails	((E2AP_ProtocolIE_ID_t)30)
#define E2AP_ProtocolIE_ID_id_TimeToWait	((E2AP_ProtocolIE_ID_t)31)
#define E2AP_ProtocolIE_ID_id_RICcontrolOutcome	((E2AP_ProtocolIE_ID_t)32)
#define E2AP_ProtocolIE_ID_id_E2nodeComponentConfigUpdate	((E2AP_ProtocolIE_ID_t)33)
#define E2AP_ProtocolIE_ID_id_E2nodeComponentConfigUpdate_Item	((E2AP_ProtocolIE_ID_t)34)
#define E2AP_ProtocolIE_ID_id_E2nodeComponentConfigUpdateAck	((E2AP_ProtocolIE_ID_t)35)
#define E2AP_ProtocolIE_ID_id_E2nodeComponentConfigUpdateAck_Item	((E2AP_ProtocolIE_ID_t)36)
#define E2AP_ProtocolIE_ID_id_E2connectionSetup	((E2AP_ProtocolIE_ID_t)39)
#define E2AP_ProtocolIE_ID_id_E2connectionSetupFailed	((E2AP_ProtocolIE_ID_t)40)
#define E2AP_ProtocolIE_ID_id_E2connectionSetupFailed_Item	((E2AP_ProtocolIE_ID_t)41)
#define E2AP_ProtocolIE_ID_id_E2connectionFailed_Item	((E2AP_ProtocolIE_ID_t)42)
#define E2AP_ProtocolIE_ID_id_E2connectionUpdate_Item	((E2AP_ProtocolIE_ID_t)43)
#define E2AP_ProtocolIE_ID_id_E2connectionUpdateAdd	((E2AP_ProtocolIE_ID_t)44)
#define E2AP_ProtocolIE_ID_id_E2connectionUpdateModify	((E2AP_ProtocolIE_ID_t)45)
#define E2AP_ProtocolIE_ID_id_E2connectionUpdateRemove	((E2AP_ProtocolIE_ID_t)46)
#define E2AP_ProtocolIE_ID_id_E2connectionUpdateRemove_Item	((E2AP_ProtocolIE_ID_t)47)
#define E2AP_ProtocolIE_ID_id_TNLinformation	((E2AP_ProtocolIE_ID_t)48)
#define E2AP_ProtocolIE_ID_id_TransactionID	((E2AP_ProtocolIE_ID_t)49)
#define E2AP_ProtocolIE_ID_id_E2nodeComponentConfigAddition	((E2AP_ProtocolIE_ID_t)50)
#define E2AP_ProtocolIE_ID_id_E2nodeComponentConfigAddition_Item	((E2AP_ProtocolIE_ID_t)51)
#define E2AP_ProtocolIE_ID_id_E2nodeComponentConfigAdditionAck	((E2AP_ProtocolIE_ID_t)52)
#define E2AP_ProtocolIE_ID_id_E2nodeComponentConfigAdditionAck_Item	((E2AP_ProtocolIE_ID_t)53)
#define E2AP_ProtocolIE_ID_id_E2nodeComponentConfigRemoval	((E2AP_ProtocolIE_ID_t)54)
#define E2AP_ProtocolIE_ID_id_E2nodeComponentConfigRemoval_Item	((E2AP_ProtocolIE_ID_t)55)
#define E2AP_ProtocolIE_ID_id_E2nodeComponentConfigRemovalAck	((E2AP_ProtocolIE_ID_t)56)
#define E2AP_ProtocolIE_ID_id_E2nodeComponentConfigRemovalAck_Item	((E2AP_ProtocolIE_ID_t)57)
#define E2AP_ProtocolIE_ID_id_E2nodeTNLassociationRemoval	((E2AP_ProtocolIE_ID_t)58)
#define E2AP_ProtocolIE_ID_id_E2nodeTNLassociationRemoval_Item	((E2AP_ProtocolIE_ID_t)59)
#define E2AP_ProtocolIE_ID_id_RICsubscriptionToBeRemoved	((E2AP_ProtocolIE_ID_t)60)
#define E2AP_ProtocolIE_ID_id_RICsubscription_withCause_Item	((E2AP_ProtocolIE_ID_t)61)

#ifdef __cplusplus
}
#endif

#endif	/* _E2AP_ProtocolIE_ID_H_ */
#include "asn_internal.h"
