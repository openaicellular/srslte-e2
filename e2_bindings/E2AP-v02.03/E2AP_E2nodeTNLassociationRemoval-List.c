/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2AP-PDU-Contents"
 * 	found in "/nexran/lib/e2ap/messages/e2ap-v02.03.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -fno-include-deps -fincludes-quoted -D /nexran/build/lib/e2ap/E2AP/`
 */

#include "E2AP_E2nodeTNLassociationRemoval-List.h"

#include "E2AP_ProtocolIE-SingleContainer.h"
asn_per_constraints_t asn_PER_type_E2AP_E2nodeTNLassociationRemoval_List_constr_1 CC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_CONSTRAINED,	 5,  5,  1,  32 }	/* (SIZE(1..32)) */,
	0, 0	/* No PER value map */
};
asn_TYPE_member_t asn_MBR_E2AP_E2nodeTNLassociationRemoval_List_1[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_E2AP_ProtocolIE_SingleContainer_1989P10,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		""
		},
};
static const ber_tlv_tag_t asn_DEF_E2AP_E2nodeTNLassociationRemoval_List_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
asn_SET_OF_specifics_t asn_SPC_E2AP_E2nodeTNLassociationRemoval_List_specs_1 = {
	sizeof(struct E2AP_E2nodeTNLassociationRemoval_List),
	offsetof(struct E2AP_E2nodeTNLassociationRemoval_List, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
asn_TYPE_descriptor_t asn_DEF_E2AP_E2nodeTNLassociationRemoval_List = {
	"E2nodeTNLassociationRemoval-List",
	"E2nodeTNLassociationRemoval-List",
	&asn_OP_SEQUENCE_OF,
	asn_DEF_E2AP_E2nodeTNLassociationRemoval_List_tags_1,
	sizeof(asn_DEF_E2AP_E2nodeTNLassociationRemoval_List_tags_1)
		/sizeof(asn_DEF_E2AP_E2nodeTNLassociationRemoval_List_tags_1[0]), /* 1 */
	asn_DEF_E2AP_E2nodeTNLassociationRemoval_List_tags_1,	/* Same as above */
	sizeof(asn_DEF_E2AP_E2nodeTNLassociationRemoval_List_tags_1)
		/sizeof(asn_DEF_E2AP_E2nodeTNLassociationRemoval_List_tags_1[0]), /* 1 */
	{ 0, &asn_PER_type_E2AP_E2nodeTNLassociationRemoval_List_constr_1, SEQUENCE_OF_constraint },
	asn_MBR_E2AP_E2nodeTNLassociationRemoval_List_1,
	1,	/* Single element */
	&asn_SPC_E2AP_E2nodeTNLassociationRemoval_List_specs_1	/* Additional specs */
};

