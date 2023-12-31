/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2AP-PDU-Contents"
 * 	found in "/nexran/lib/e2ap/messages/e2ap-v02.03.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -fno-include-deps -fincludes-quoted -D /nexran/build/lib/e2ap/E2AP/`
 */

#include "E2AP_RANfunction-Item.h"

asn_TYPE_member_t asn_MBR_E2AP_RANfunction_Item_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct E2AP_RANfunction_Item, ranFunctionID),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_E2AP_RANfunctionID,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"ranFunctionID"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct E2AP_RANfunction_Item, ranFunctionDefinition),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_E2AP_RANfunctionDefinition,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"ranFunctionDefinition"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct E2AP_RANfunction_Item, ranFunctionRevision),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_E2AP_RANfunctionRevision,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"ranFunctionRevision"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct E2AP_RANfunction_Item, ranFunctionOID),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_E2AP_RANfunctionOID,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"ranFunctionOID"
		},
};
static const ber_tlv_tag_t asn_DEF_E2AP_RANfunction_Item_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_E2AP_RANfunction_Item_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* ranFunctionID */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* ranFunctionDefinition */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* ranFunctionRevision */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* ranFunctionOID */
};
asn_SEQUENCE_specifics_t asn_SPC_E2AP_RANfunction_Item_specs_1 = {
	sizeof(struct E2AP_RANfunction_Item),
	offsetof(struct E2AP_RANfunction_Item, _asn_ctx),
	asn_MAP_E2AP_RANfunction_Item_tag2el_1,
	4,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	4,	/* First extension addition */
};
asn_TYPE_descriptor_t asn_DEF_E2AP_RANfunction_Item = {
	"RANfunction-Item",
	"RANfunction-Item",
	&asn_OP_SEQUENCE,
	asn_DEF_E2AP_RANfunction_Item_tags_1,
	sizeof(asn_DEF_E2AP_RANfunction_Item_tags_1)
		/sizeof(asn_DEF_E2AP_RANfunction_Item_tags_1[0]), /* 1 */
	asn_DEF_E2AP_RANfunction_Item_tags_1,	/* Same as above */
	sizeof(asn_DEF_E2AP_RANfunction_Item_tags_1)
		/sizeof(asn_DEF_E2AP_RANfunction_Item_tags_1[0]), /* 1 */
	{ 0, 0, SEQUENCE_constraint },
	asn_MBR_E2AP_RANfunction_Item_1,
	4,	/* Elements count */
	&asn_SPC_E2AP_RANfunction_Item_specs_1	/* Additional specs */
};

