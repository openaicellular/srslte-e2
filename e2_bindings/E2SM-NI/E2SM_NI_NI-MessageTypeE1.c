/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2SM-NI-IEs"
 * 	found in "/local/mnt/openairinterface5g/openair2/RIC_AGENT/MESSAGES/ASN1/R01/e2sm-ni-v01.00.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -fno-include-deps -fincludes-quoted -D /local/mnt/openairinterface5g/cmake_targets/ran_build/build/CMakeFiles/E2SM-NI/`
 */

#include "E2SM_NI_NI-MessageTypeE1.h"

/*
 * This type is implemented using E2SM_NI_NI_MessageTypeApproach1,
 * so here we adjust the DEF accordingly.
 */
static const ber_tlv_tag_t asn_DEF_E2SM_NI_NI_MessageTypeE1_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
asn_TYPE_descriptor_t asn_DEF_E2SM_NI_NI_MessageTypeE1 = {
	"NI-MessageTypeE1",
	"NI-MessageTypeE1",
	&asn_OP_SEQUENCE,
	asn_DEF_E2SM_NI_NI_MessageTypeE1_tags_1,
	sizeof(asn_DEF_E2SM_NI_NI_MessageTypeE1_tags_1)
		/sizeof(asn_DEF_E2SM_NI_NI_MessageTypeE1_tags_1[0]), /* 1 */
	asn_DEF_E2SM_NI_NI_MessageTypeE1_tags_1,	/* Same as above */
	sizeof(asn_DEF_E2SM_NI_NI_MessageTypeE1_tags_1)
		/sizeof(asn_DEF_E2SM_NI_NI_MessageTypeE1_tags_1[0]), /* 1 */
	{ 0, 0, SEQUENCE_constraint },
	asn_MBR_E2SM_NI_NI_MessageTypeApproach1_1,
	2,	/* Elements count */
	&asn_SPC_E2SM_NI_NI_MessageTypeApproach1_specs_1	/* Additional specs */
};

