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

//
// defines the data structure to hold the details of the PCI Root port devices
//
typedef struct _PRIMARY_ROOT_PORT_NODE  PRIMARY_ROOT_PORT_NODE;

//
// defines the data structure to hold the configuration data for the other PCI
// features
//
typedef struct _OTHER_PCI_FEATURES_CONFIGURATION_TABLE  OTHER_PCI_FEATURES_CONFIGURATION_TABLE;

//
// Defines for the PCI features configuration completion and re-enumeration list
//
typedef struct _PCI_FEATURE_CONFIGURATION_COMPLETION_LIST  PCI_FEATURE_CONFIGURATION_COMPLETION_LIST;

//
// Signature value for the PCI Root Port node
//
#define PCI_ROOT_PORT_SIGNATURE               SIGNATURE_32 ('p', 'c', 'i', 'p')

//
// Definitions of the PCI Root Port data structure members
//
struct _PRIMARY_ROOT_PORT_NODE {
  //
  // Signature header
  //
  UINT32                                    Signature;
  //
  // linked list pointers to next node
  //
  LIST_ENTRY                                NeighborRootPort;
  //
  // pointer to PCI_IO_DEVICE of the primary PCI Controller device
  //
  EFI_DEVICE_PATH_PROTOCOL                  *RootPortDevicePath;
  //
  // pointer to the corresponding PCI feature configuration Table node
  // all the child PCI devices of the controller are aligned based on this table
  //
  OTHER_PCI_FEATURES_CONFIGURATION_TABLE    *OtherPciFeaturesConfigurationTable;
};

#define PRIMARY_ROOT_PORT_NODE_FROM_LINK(a) \
  CR (a, PRIMARY_ROOT_PORT_NODE, NeighborRootPort, PCI_ROOT_PORT_SIGNATURE)

//
// Definition of the PCI Feature configuration Table members
//
struct _OTHER_PCI_FEATURES_CONFIGURATION_TABLE {
  //
  // Configuration Table ID
  //
  UINTN                                     ID;
};


//
// PCI feature configuration node signature value
//
#define PCI_FEATURE_CONFIGURATION_SIGNATURE               SIGNATURE_32 ('p', 'c', 'i', 'f')

struct _PCI_FEATURE_CONFIGURATION_COMPLETION_LIST {
  //
  // Signature header
  //
  UINT32                                    Signature;
  //
  // link to next Root Bridge whose PCI Feature configuration is complete
  //
  LIST_ENTRY                                RootBridgeLink;
  //
  // EFI handle of the Root Bridge whose PCI feature configuration is complete
  //
  EFI_HANDLE                                RootBridgeHandle;
  //
  // indication for complete re-enumeration of the PCI feature configuration
  //
  BOOLEAN                                   ReEnumeratePciFeatureConfiguration;
};

#define PCI_FEATURE_CONFIGURATION_COMPLETION_LIST_FROM_LINK(a) \
  CR (a, PCI_FEATURE_CONFIGURATION_COMPLETION_LIST, RootBridgeLink, PCI_FEATURE_CONFIGURATION_SIGNATURE)

//
// Declaration of the internal sub-phases within the PCI Feature enumeration
//
typedef enum {
  //
  // initial phase in configuring the other PCI features to record the primary
  // root ports
  //
  PciFeatureRootBridgeScan,
  //
  // get the PCI device-specific platform policies and align with device capabilities
  //
  PciFeatureGetDevicePolicy,
  //
  // align all PCI nodes in the PCI heirarchical tree
  //
  PciFeatureSetupPhase,
  //
  // finally override to complete configuration of the PCI feature
  //
  PciFeatureConfigurationPhase,
  //
  // PCI feature configuration complete
  //
  PciFeatureConfigurationComplete

}PCI_FEATURE_CONFIGURATION_PHASE;


/**
  Main routine to indicate platform selection of any of the other PCI features
  to be configured by this driver

  @retval TRUE    platform has selected the other PCI features to be configured
          FALSE   platform has not selected any of the other PCI features
**/
BOOLEAN
CheckOtherPciFeaturesPcd (
  );

/**
  Enumerate all the nodes of the specified root bridge or PCI-PCI Bridge, to
  configure the other PCI features.

  @param RootBridge          A pointer to the PCI_IO_DEVICE.

  @retval EFI_SUCCESS           The other PCI features configuration during enumeration
                                of all the nodes of the PCI root bridge instance were
                                programmed in PCI-compliance pattern along with the
                                device-specific policy, as applicable.
  @retval EFI_UNSUPPORTED       One of the override operation maong the nodes of
                                the PCI hierarchy resulted in a incompatible address
                                range.
  @retval EFI_INVALID_PARAMETER The override operation is performed with invalid input
                                parameters.
**/
EFI_STATUS
EnumerateOtherPciFeatures (
  IN PCI_IO_DEVICE           *RootBridge
  );

/**
  This routine is invoked from the Stop () interface for the EFI handle of the
  RootBridge. Free up its node of type PCI_FEATURE_CONFIGURATION_COMPLETION_LIST.

  @param  RootBridge      A pointer to the PCI_IO_DEVICE
**/
VOID
DestroyRootBridgePciFeaturesConfigCompletionList (
  IN PCI_IO_DEVICE          *RootBridge
  );
#endif
