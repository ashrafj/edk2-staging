/** @file
  PCI standard feature support functions implementation for PCI Bus module..

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _EFI_PCI_FEATURES_SUPPORT_H_
#define _EFI_PCI_FEATURES_SUPPORT_H_
//
// Macro definitions for the PCI Features support PCD
//
#define PCI_FEATURE_SUPPORT_FLAG_MPS  BIT0
#define PCI_FEATURE_SUPPORT_FLAG_MRRS BIT1
#define PCI_FEATURE_SUPPORT_FLAG_RO   BIT2
#define PCI_FEATURE_SUPPORT_FLAG_NS   BIT3
#define PCI_FEATURE_SUPPORT_FLAG_CTO  BIT4
#define PCI_FEATURE_SUPPORT_FLAG_ETAG BIT5
#define PCI_FEATURE_SUPPORT_FLAG_AOP  BIT6
#define PCI_FEATURE_SUPPORT_FLAG_LTR  BIT7
#define PCI_FEATURE_SUPPORT_FLAG_ASPM BIT12
#define PCI_FEATURE_SUPPORT_FLAG_CCC  BIT13
#define PCI_FEATURE_SUPPORT_FLAG_ESYN BIT14
#define PCI_FEATURE_SUPPORT_FLAG_PTM  BIT20
#endif
