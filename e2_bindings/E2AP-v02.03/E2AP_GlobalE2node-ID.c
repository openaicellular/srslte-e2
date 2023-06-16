/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2AP-IEs"
 * 	found in "/nexran/lib/e2ap/messages/e2ap-v02.03.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -fno-include-deps -fincludes-quoted -D /nexran/build/lib/e2ap/E2AP/`
 */

#include "E2AP_GlobalE2node-ID.h"

asn_per_constraints_t asn_PER_type_E2AP_GlobalE2node_ID_constr_1 CC_NOTUSED = {
	{ APC_CONSTRAINED | APC_EXTENSIBLE,  2,  2,  0,  3 }	/* (0..3,...) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
asn_TYPE_member_t asn_MBR_E2AP_GlobalE2node_ID_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct E2AP_GlobalE2node_ID, choice.gNB),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_E2AP_GlobalE2node_gNB_ID,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"gNB"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct E2AP_GlobalE2node_ID, choice.en_gNB),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_E2AP_GlobalE2node_en_gNB_ID,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"en-gNB"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct E2AP_GlobalE2node_ID, choice.ng_eNB),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_E2AP_GlobalE2node_ng_eNB_ID,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"ng-eNB"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct E2AP_GlobalE2node_ID, choice.eNB),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_E2AP_GlobalE2node_eNB_ID,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"eNB"
		},
};
static const asn_TYPE_tag2member_t asn_MAP_E2AP_GlobalE2node_ID_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* gNB */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* en-gNB */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* ng-eNB */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* eNB */
};
asn_CHOICE_specifics_t asn_SPC_E2AP_GlobalE2node_ID_specs_1 = {
	sizeof(struct E2AP_GlobalE2node_ID),
	offsetof(struct E2AP_GlobalE2node_ID, _asn_ctx),
	offsetof(struct E2AP_GlobalE2node_ID, present),
	sizeof(((struct E2AP_GlobalE2node_ID *)0)->present),
	asn_MAP_E2AP_GlobalE2node_ID_tag2el_1,
	4,	/* Count of tags in the map */
	0, 0,
	4	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_E2AP_GlobalE2node_ID = {
	"GlobalE2node-ID",
	"GlobalE2node-ID",
	&asn_OP_CHOICE,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	{ 0, &asn_PER_type_E2AP_GlobalE2node_ID_constr_1, CHOICE_constraint },
	asn_MBR_E2AP_GlobalE2node_ID_1,
	4,	/* Elements count */
	&asn_SPC_E2AP_GlobalE2node_ID_specs_1	/* Additional specs */
};
