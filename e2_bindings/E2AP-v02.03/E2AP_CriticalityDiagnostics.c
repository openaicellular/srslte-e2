/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2AP-IEs"
 * 	found in "/nexran/lib/e2ap/messages/e2ap-v02.03.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -fno-include-deps -fincludes-quoted -D /nexran/build/lib/e2ap/E2AP/`
 */

#include "E2AP_CriticalityDiagnostics.h"

#include "E2AP_RICrequestID.h"
#include "E2AP_CriticalityDiagnostics-IE-List.h"
asn_TYPE_member_t asn_MBR_E2AP_CriticalityDiagnostics_1[] = {
	{ ATF_POINTER, 5, offsetof(struct E2AP_CriticalityDiagnostics, procedureCode),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_E2AP_ProcedureCode,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"procedureCode"
		},
	{ ATF_POINTER, 4, offsetof(struct E2AP_CriticalityDiagnostics, triggeringMessage),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_E2AP_TriggeringMessage,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"triggeringMessage"
		},
	{ ATF_POINTER, 3, offsetof(struct E2AP_CriticalityDiagnostics, procedureCriticality),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_E2AP_Criticality,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"procedureCriticality"
		},
	{ ATF_POINTER, 2, offsetof(struct E2AP_CriticalityDiagnostics, ricRequestorID),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_E2AP_RICrequestID,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"ricRequestorID"
		},
	{ ATF_POINTER, 1, offsetof(struct E2AP_CriticalityDiagnostics, iEsCriticalityDiagnostics),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_E2AP_CriticalityDiagnostics_IE_List,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"iEsCriticalityDiagnostics"
		},
};
static const int asn_MAP_E2AP_CriticalityDiagnostics_oms_1[] = { 0, 1, 2, 3, 4 };
static const ber_tlv_tag_t asn_DEF_E2AP_CriticalityDiagnostics_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_E2AP_CriticalityDiagnostics_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* procedureCode */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* triggeringMessage */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* procedureCriticality */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* ricRequestorID */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 } /* iEsCriticalityDiagnostics */
};
asn_SEQUENCE_specifics_t asn_SPC_E2AP_CriticalityDiagnostics_specs_1 = {
	sizeof(struct E2AP_CriticalityDiagnostics),
	offsetof(struct E2AP_CriticalityDiagnostics, _asn_ctx),
	asn_MAP_E2AP_CriticalityDiagnostics_tag2el_1,
	5,	/* Count of tags in the map */
	asn_MAP_E2AP_CriticalityDiagnostics_oms_1,	/* Optional members */
	5, 0,	/* Root/Additions */
	5,	/* First extension addition */
};
asn_TYPE_descriptor_t asn_DEF_E2AP_CriticalityDiagnostics = {
	"CriticalityDiagnostics",
	"CriticalityDiagnostics",
	&asn_OP_SEQUENCE,
	asn_DEF_E2AP_CriticalityDiagnostics_tags_1,
	sizeof(asn_DEF_E2AP_CriticalityDiagnostics_tags_1)
		/sizeof(asn_DEF_E2AP_CriticalityDiagnostics_tags_1[0]), /* 1 */
	asn_DEF_E2AP_CriticalityDiagnostics_tags_1,	/* Same as above */
	sizeof(asn_DEF_E2AP_CriticalityDiagnostics_tags_1)
		/sizeof(asn_DEF_E2AP_CriticalityDiagnostics_tags_1[0]), /* 1 */
	{ 0, 0, SEQUENCE_constraint },
	asn_MBR_E2AP_CriticalityDiagnostics_1,
	5,	/* Elements count */
	&asn_SPC_E2AP_CriticalityDiagnostics_specs_1	/* Additional specs */
};

