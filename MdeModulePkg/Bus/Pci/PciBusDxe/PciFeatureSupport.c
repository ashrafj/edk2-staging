/** @file
  PCI standard feature support functions implementation for PCI Bus module..

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PciBus.h"
#include "PciFeatureSupport.h"

/**
  A gobal pointer to PRIMARY_ROOT_PORT_NODE buffer to track all the primary physical
  PCI Root Ports (PCI Controllers) for a given PCI Root Bridge instance while
  enumerating to configure the PCI features
**/
PRIMARY_ROOT_PORT_NODE                      *mPrimaryRootPortList;

/**
  A global pointer to PCI_FEATURE_CONFIGURATION_COMPLETION_LIST, which stores all
  the PCI Root Bridge instances that are enumerated for the other PCI features,
  like MaxPayloadSize & MaxReadReqSize; during the the Start() interface of the
  driver binding protocol. The records pointed by this pointer would be destroyed
  when the DXE core invokes the Stop() interface.
**/
PCI_FEATURE_CONFIGURATION_COMPLETION_LIST   *mPciFeaturesConfigurationCompletionList = NULL;

/**
  Main routine to indicate platform selection of any of the other PCI features
  to be configured by this driver

  @retval TRUE    platform has selected the other PCI features to be configured
          FALSE   platform has not selected any of the other PCI features
**/
BOOLEAN
CheckOtherPciFeaturesPcd (
  )
{
  return PcdGet32 (PcdOtherPciFeatures) ? TRUE : FALSE;
}

/**
  Main routine to indicate whether the platform has selected the Max_Payload_Size
  PCI feature to be configured by this driver

  @retval TRUE    platform has selected the Max_Payload_Size to be configured
          FALSE   platform has not selected this feature
**/
BOOLEAN
SetupMaxPayloadSize (
  )
{
  return (PcdGet32 (PcdOtherPciFeatures) & PCI_FEATURE_SUPPORT_FLAG_MPS) ? TRUE : FALSE;
}

/**
  Main routine to indicate whether the platform has selected the Max_Read_Req_Size
  PCI feature to be configured by this driver

  @retval TRUE    platform has selected the Max_Read_Req_Size to be configured
          FALSE   platform has not selected this feature
**/
BOOLEAN
SetupMaxReadReqSize (
  )
{
  return (PcdGet32 (PcdOtherPciFeatures) & PCI_FEATURE_SUPPORT_FLAG_MRRS) ? TRUE : FALSE;
}

/**
  Main routine to indicate whether the platform has selected the Relax Ordering
  PCI feature to be configured by this driver

  @retval TRUE    platform has selected the Relax Ordering to be configured
          FALSE   platform has not selected this feature
**/
BOOLEAN
SetupRelaxOrder (
  )
{
  return (PcdGet32 (PcdOtherPciFeatures) & PCI_FEATURE_SUPPORT_FLAG_RO) ? TRUE : FALSE;
}

/**
  Main routine to indicate whether the platform has selected the No-Snoop
  PCI feature to be configured by this driver

  @retval TRUE    platform has selected the No-Snoop to be configured
          FALSE   platform has not selected this feature
**/
BOOLEAN
SetupNoSnoop (
  )
{
  return (PcdGet32 (PcdOtherPciFeatures) & PCI_FEATURE_SUPPORT_FLAG_NS) ? TRUE : FALSE;
}

/**
  Main routine to indicate whether the platform has selected the Completion Timeout
  PCI feature to be configured by this driver

  @retval TRUE    platform has selected the Completion Timeout to be configured
          FALSE   platform has not selected this feature
**/
BOOLEAN
SetupCompletionTimeout (
  )
{
  return (PcdGet32 (PcdOtherPciFeatures) & PCI_FEATURE_SUPPORT_FLAG_CTO) ? TRUE : FALSE;
}

/**
  Main routine to indicate whether the platform has selected the Extended Tag
  PCI feature to be configured by this driver

  @retval TRUE    platform has selected the Completion Timeout to be configured
          FALSE   platform has not selected this feature
**/
BOOLEAN
SetupExtendedTag (
  )
{
  return (PcdGet32 (PcdOtherPciFeatures) & PCI_FEATURE_SUPPORT_FLAG_ETAG) ? TRUE : FALSE;
}

/**
  Main routine to indicate whether the platform has selected the Atomic Op
  PCI feature to be configured by this driver

  @retval TRUE    platform has selected the Completion Timeout to be configured
          FALSE   platform has not selected this feature
**/
BOOLEAN
SetupAtomicOp (
  )
{
  return (PcdGet32 (PcdOtherPciFeatures) & PCI_FEATURE_SUPPORT_FLAG_AOP) ? TRUE : FALSE;
}
/**
  Main routine to indicate whether the platform has selected the LTR
  PCI feature to be configured by this driver

  @retval TRUE    platform has selected the Completion Timeout to be configured
          FALSE   platform has not selected this feature
**/
BOOLEAN
SetupLtr (
  )
{
  return (PcdGet32 (PcdOtherPciFeatures) & PCI_FEATURE_SUPPORT_FLAG_LTR) ? TRUE : FALSE;
}

/**
  Main routine to indicate whether the platform has selected the ASPM state
  PCI feature to be configured by this driver

  @retval TRUE    platform has selected the Completion Timeout to be configured
          FALSE   platform has not selected this feature
**/
BOOLEAN
SetupAspm (
  )
{
  return (PcdGet32 (PcdOtherPciFeatures) & PCI_FEATURE_SUPPORT_FLAG_ASPM) ? TRUE : FALSE;
}

/**
  Main routine to indicate whether the platform has selected the Common Clock Configuration
  PCI feature to be configured by this driver

  @retval TRUE    platform has selected the Completion Timeout to be configured
          FALSE   platform has not selected this feature
**/
BOOLEAN
SetupCommonClkCfg (
  )
{
  return (PcdGet32 (PcdOtherPciFeatures) & PCI_FEATURE_SUPPORT_FLAG_CCC) ? TRUE : FALSE;
}

/**
  Main routine to indicate whether the platform has selected the Extended Synch
  PCI feature to be configured by this driver

  @retval TRUE    platform has selected the Completion Timeout to be configured
          FALSE   platform has not selected this feature
**/
BOOLEAN
SetupExtendedSynch (
  )
{
  return (PcdGet32 (PcdOtherPciFeatures) & PCI_FEATURE_SUPPORT_FLAG_ESYN) ? TRUE : FALSE;
}

/**
  Main routine to indicate whether the platform has selected the PIM Control
  PCI feature to be configured by this driver

  @retval TRUE    platform has selected the Completion Timeout to be configured
          FALSE   platform has not selected this feature
**/
BOOLEAN
SetupPtm (
  )
{
  return (PcdGet32 (PcdOtherPciFeatures) & PCI_FEATURE_SUPPORT_FLAG_PTM) ? TRUE : FALSE;
}

/**
 Helper routine to determine the existence of previously enumerated PCI device

 @retval  TRUE  PCI device exist
          FALSE does not exist
**/
BOOLEAN
DeviceExist (
  PCI_IO_DEVICE                   *PciDevice
  )
{
  EFI_PCI_IO_PROTOCOL   *PciIoProtocol = &PciDevice->PciIo;
  UINT16                VendorId = 0xFFFF;

  PciIoProtocol->Pci.Read (
                      PciIoProtocol,
                      EfiPciIoWidthUint16,
                      PCI_VENDOR_ID_OFFSET,
                      1,
                      &VendorId
                      );
  if (VendorId == 0 || VendorId == 0xFFFF) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/**
  Helper routine which determines whether the given PCI Root Bridge instance
  record already exist. This routine shall help avoid duplicate record creation
  in case of re-enumeration of PCI configuation features.

  @param  RootBridge              A pointer to the PCI_IO_DEVICE for the Root Bridge
  @param  PciFeatureConfigRecord  A pointer to a pointer for type
                                  PCI_FEATURE_CONFIGURATION_COMPLETION_LIST
                                  record, Use to return the specific record.

  @retval TRUE                    Record already exist
          FALSE                   Record does not exist for the given PCI Root Bridge
**/
BOOLEAN
CheckPciFeatureConfigurationRecordExist (
  IN  PCI_IO_DEVICE                             *RootBridge,
  OUT PCI_FEATURE_CONFIGURATION_COMPLETION_LIST **PciFeatureConfigRecord
  )
{
  LIST_ENTRY                                  *Link;
  PCI_FEATURE_CONFIGURATION_COMPLETION_LIST   *Temp;

  if (mPciFeaturesConfigurationCompletionList) {
    Link = &mPciFeaturesConfigurationCompletionList->RootBridgeLink;

    do {
      Temp = PCI_FEATURE_CONFIGURATION_COMPLETION_LIST_FROM_LINK (Link);
      if (Temp->RootBridgeHandle == RootBridge->Handle) {
        *PciFeatureConfigRecord = Temp;
        return TRUE;
      }
      Link = Link->ForwardLink;
    } while (Link != &mPciFeaturesConfigurationCompletionList->RootBridgeLink);
  }
  //
  // not found on the PCI feature configuration completion list
  //
  *PciFeatureConfigRecord = NULL;
  return FALSE;
}

/**
  This routine is primarily to avoid multiple configuration of PCI features
  to the same PCI Root Bridge due to EDK2 core's ConnectController calls on
  all the EFI handles. This routine also provide re-enumeration of the PCI
  features on the same PCI Root Bridge based on the policy of ReEnumeratePciFeatureConfiguration
  of the PCI_FEATURE_CONFIGURATION_COMPLETION_LIST.

  @param  RootBridge              A pointer to the PCI_IO_DEVICE for the Root Bridge

  @retval TRUE                    PCI Feature configuration required for the PCI
                                  Root Bridge
          FALSE                   PCI Feature configuration is not required to be
                                  re-enumerated for the PCI Root Bridge
**/
BOOLEAN
CheckPciFeaturesConfigurationRequired (
  IN PCI_IO_DEVICE          *RootBridge
  )
{
  LIST_ENTRY                                  *Link;
  PCI_FEATURE_CONFIGURATION_COMPLETION_LIST   *Temp;

  if (mPciFeaturesConfigurationCompletionList) {
    Link = &mPciFeaturesConfigurationCompletionList->RootBridgeLink;

    do {
      Temp = PCI_FEATURE_CONFIGURATION_COMPLETION_LIST_FROM_LINK (Link);
      if (Temp->RootBridgeHandle == RootBridge->Handle) {
        return Temp->ReEnumeratePciFeatureConfiguration;
      }
      Link = Link->ForwardLink;
    } while (Link != &mPciFeaturesConfigurationCompletionList->RootBridgeLink);
  }
  //
  // not found on the PCI feature configuration completion list, return as required
  //
  return TRUE;
}

/**
  This routine finds the duplicate record if exist and assigns the re-enumeration
  requirement flag, as passed as input. It creates new record for the PCI Root
  Bridge and appends the list after updating its re-enumeration flag.

  @param  RootBridge            A pointer to PCI_IO_DEVICE of the Root Bridge
  @param  ReEnumerationRequired A BOOLEAN for recording the re-enumeration requirement

  @retval EFI_SUCCESS           new record inserted into the list or updated the
                                existing record
          EFI_INVALID_PARAMETER Unexpected error as CheckPciFeatureConfigurationRecordExist
                                reports as record exist but does not return its pointer
          EFI_OUT_OF_RESOURCES  Not able to create PCI features configuratin complete
                                record for the RootBridge
**/
EFI_STATUS
AddRootBridgeInPciFeaturesConfigCompletionList (
  IN PCI_IO_DEVICE          *RootBridge,
  IN BOOLEAN                ReEnumerationRequired
  )
{
  PCI_FEATURE_CONFIGURATION_COMPLETION_LIST   *Temp;

  if (CheckPciFeatureConfigurationRecordExist (RootBridge, &Temp)) {
    //
    // this PCI Root Bridge record already exist; it may have been re-enumerated
    // hence just update its enumeration required flag again to exit
    //
    if (Temp) {
      Temp->ReEnumeratePciFeatureConfiguration  = ReEnumerationRequired;
      return EFI_SUCCESS;
    } else {
      //
      // PCI feature configuration complete record reported as exist and no
      // record pointer returned
      //
      return EFI_INVALID_PARAMETER;
    }

  } else {

    Temp = AllocateZeroPool (sizeof (PCI_FEATURE_CONFIGURATION_COMPLETION_LIST));
    if (Temp == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    Temp->Signature                           = PCI_FEATURE_CONFIGURATION_SIGNATURE;
    Temp->RootBridgeHandle                    = RootBridge->Handle;
    Temp->ReEnumeratePciFeatureConfiguration  = ReEnumerationRequired;
    if (mPciFeaturesConfigurationCompletionList) {
      InsertTailList (
          &mPciFeaturesConfigurationCompletionList->RootBridgeLink,
          &Temp->RootBridgeLink
          );
    } else {
      //
      // init the very first node of the Root Bridge
      //
      mPciFeaturesConfigurationCompletionList = Temp;
      InitializeListHead (&mPciFeaturesConfigurationCompletionList->RootBridgeLink);
    }
  }
  return EFI_SUCCESS;
}

/**
  Free up memory alloted for the primary physical PCI Root ports of the PCI Root
  Bridge instance. Free up all the nodes of type PRIMARY_ROOT_PORT_NODE.
**/
VOID
DestroyPrimaryRootPortNodes ()
{
  LIST_ENTRY                *Link;
  PRIMARY_ROOT_PORT_NODE    *Temp;

  if (mPrimaryRootPortList) {
    Link = &mPrimaryRootPortList->NeighborRootPort;

    if (IsListEmpty (Link)) {
      FreePool (mPrimaryRootPortList->OtherPciFeaturesConfigurationTable);
      FreePool (mPrimaryRootPortList);
    } else {
      do {
        if (Link->ForwardLink != &mPrimaryRootPortList->NeighborRootPort) {
          Link = Link->ForwardLink;
        }
        Temp = PRIMARY_ROOT_PORT_NODE_FROM_LINK (Link);
        Link = RemoveEntryList (Link);
        FreePool (Temp->OtherPciFeaturesConfigurationTable);
        FreePool (Temp);
      } while (!IsListEmpty (Link));
      FreePool (mPrimaryRootPortList->OtherPciFeaturesConfigurationTable);
      FreePool (mPrimaryRootPortList);
    }
    mPrimaryRootPortList = NULL;
  }
}

/**
  Routine meant for initializing any global variables used. It primarily cleans
  up the internal data structure memory allocated for the previous PCI Root Bridge
  instance. This should be the first routine to call for any virtual PCI Root
  Bridge instance.
**/
VOID
SetupPciFeaturesConfigurationDefaults ()
{
  //
  // delete the primary root port list
  //
  if (mPrimaryRootPortList) {
    DestroyPrimaryRootPortNodes ();
  }
}

/**
  Main routine to determine the child PCI devices of a PCI bridge device
  and group them under a common internal PCI features Configuration table.

  @param  PciDevice                       A pointer to the PCI_IO_DEVICE.
  @param  PciFeaturesConfigTable          A pointer to a pointer to the
                                          OTHER_PCI_FEATURES_CONFIGURATION_TABLE.
                                          Returns NULL in case of RCiEP or the PCI
                                          device does match with any of the physical
                                          Root ports, or it does not belong to any
                                          Root port's PCI bus range (not a child)

  @retval EFI_SUCCESS                     able to determine the PCI feature
                                          configuration table. For RCiEP since
                                          since it is not prepared.
          EFI_DEVICE_ERROR                the PCI device has invalid EFI device
                                          path
**/
EFI_STATUS
GetPciFeaturesConfigurationTable (
  IN  PCI_IO_DEVICE                           *PciDevice,
  OUT OTHER_PCI_FEATURES_CONFIGURATION_TABLE  **PciFeaturesConfigTable
  )
{
  LIST_ENTRY                *Link;
  PRIMARY_ROOT_PORT_NODE    *Temp;
  BOOLEAN                   NodeMatch;
  EFI_DEVICE_PATH_PROTOCOL  *RootPortPath;
  EFI_DEVICE_PATH_PROTOCOL  *PciDevicePath;

  if (mPrimaryRootPortList == NULL) {
    //
    // no populated PCI primary root ports to parse and match the PCI features
    // configuration table
    //
    *PciFeaturesConfigTable = NULL;
    return EFI_SUCCESS;
  }

  //
  // The PCI features configuration table is not built for RCiEP, return NULL
  //
  if (PciDevice->PciExpStruct.Capability.Bits.DevicePortType == \
      PCIE_DEVICE_PORT_TYPE_ROOT_COMPLEX_INTEGRATED_ENDPOINT) {
    *PciFeaturesConfigTable = NULL;
    return EFI_SUCCESS;
  }

  if (IsDevicePathEnd (PciDevice->DevicePath)){
    //
    // the given PCI device does not have a valid device path
    //
    *PciFeaturesConfigTable = NULL;
    return EFI_DEVICE_ERROR;
  }


  Link = &mPrimaryRootPortList->NeighborRootPort;
  do {
    Temp = PRIMARY_ROOT_PORT_NODE_FROM_LINK (Link);
    RootPortPath = Temp->RootPortDevicePath;
    PciDevicePath = PciDevice->DevicePath;
    NodeMatch = FALSE;
    //
    // match the device path from the list of primary Root Ports with the given
    // device; the initial nodes matching in sequence indicate that the given PCI
    // device belongs to that PCI tree from the root port
    //
    if (IsDevicePathEnd (RootPortPath)) {
      //
      // critical error as no device path available in root
      //
      *PciFeaturesConfigTable = NULL;
      return EFI_DEVICE_ERROR;
    }

    if (EfiCompareDevicePath (RootPortPath, PciDevicePath)) {
      //
      // the given PCI device is the primary root port itself
      //
      *PciFeaturesConfigTable = Temp->OtherPciFeaturesConfigurationTable;
      return EFI_SUCCESS;
    }
    //
    // check this PCI device belongs to the primary root port of the root bridge
    // any child PCI device will have the same initial device path nodes  as
    // its parent root port
    //
    while (!IsDevicePathEnd (RootPortPath)){

      if (DevicePathNodeLength (RootPortPath) != DevicePathNodeLength (PciDevicePath)) {
        //
        // break to check the next primary root port nodes as does not match
        //
        NodeMatch = FALSE;
        break;
      }
      if (CompareMem (RootPortPath, PciDevicePath, DevicePathNodeLength (RootPortPath)) != 0) {
        //
        // node does not match, break to check next node
        //
        NodeMatch = FALSE;
        break;
      }
      NodeMatch = TRUE;
      //
      // advance to next node
      //
      RootPortPath = NextDevicePathNode (RootPortPath);
      PciDevicePath = NextDevicePathNode (PciDevicePath);
    }

    if (NodeMatch == TRUE) {
      //
      // device belongs to primary root port, return its PCI feature configuration
      // table
      //
      *PciFeaturesConfigTable = Temp->OtherPciFeaturesConfigurationTable;
      return EFI_SUCCESS;
    }

    //
    // advance to next Root port node
    //
    Link = Link->ForwardLink;
  } while (Link != &mPrimaryRootPortList->NeighborRootPort);
  //
  // the PCI device must be RCiEP, does not belong to any primary root port
  //
  *PciFeaturesConfigTable = NULL;
  return EFI_SUCCESS;
}

/**
  This routine determines the existance of the child PCI device for the given
  PCI Root / Bridge Port device. Always assumes the input PCI device is the bridge
  or PCI-PCI Bridge device. This routine should not be used with PCI endpoint device.

  @param  PciDevice                       A pointer to the PCI_IO_DEVICE.

  @retval TRUE                            child device exist
          FALSE                           no child device
**/
BOOLEAN
IsPciRootPortEmpty (
  IN  PCI_IO_DEVICE                           *PciDevice
  )
{
  if (IsListEmpty (&PciDevice->ChildList)){
    return TRUE;
  }
  return FALSE;
}

/**
  The main routine which process the PCI feature Max_Payload_Size as per the
  device-specific platform policy, as well as in complaince with the PCI Base
  specification Revision 4, that aligns the value for the entire PCI heirarchy
  starting from its physical PCI Root port / Bridge device.

  @param PciDevice                      A pointer to the PCI_IO_DEVICE.
  @param PciConfigPhase                 for the PCI feature configuration phases:
                                        PciFeatureGetDevicePolicy & PciFeatureSetupPhase
  @param PciFeaturesConfigurationTable  pointer to OTHER_PCI_FEATURES_CONFIGURATION_TABLE

  @retval EFI_SUCCESS                   processing of PCI feature Max_Payload_Size
                                        is successful.
**/
EFI_STATUS
ProcessMaxPayloadSize (
  IN  PCI_IO_DEVICE                           *PciDevice,
  IN  PCI_FEATURE_CONFIGURATION_PHASE         PciConfigPhase,
  IN  OTHER_PCI_FEATURES_CONFIGURATION_TABLE  *PciFeaturesConfigurationTable
  )
{
  PCI_REG_PCIE_DEVICE_CAPABILITY          PciDeviceCap;
  UINT8                                   MpsValue;


  PciDeviceCap.Uint32 = PciDevice->PciExpStruct.DeviceCapability.Uint32;

  if (PciConfigPhase == PciFeatureGetDevicePolicy) {
    if (SetupMpsAsPerDeviceCapability (PciDevice->SetupMPS)) {
      MpsValue = (UINT8)PciDeviceCap.Bits.MaxPayloadSize;
      //
      // no change to PCI Root ports without any endpoint device
      //
      if (IS_PCI_BRIDGE (&PciDevice->Pci) && PciDeviceCap.Bits.MaxPayloadSize) {
        if (IsPciRootPortEmpty (PciDevice)) {
          MpsValue = PCIE_MAX_PAYLOAD_SIZE_128B;
        }
      }
    } else {
      MpsValue = TranslateMpsSetupValueToPci (PciDevice->SetupMPS);
    }
    //
    // discard device policy override request if greater than PCI device capability
    //
    PciDevice->SetupMPS = MIN ((UINT8)PciDeviceCap.Bits.MaxPayloadSize, MpsValue);
  }

  //
  // align the MPS of the tree to the HCF with this device
  //
  if (PciFeaturesConfigurationTable) {
    MpsValue = PciFeaturesConfigurationTable->Max_Payload_Size;

    MpsValue = MIN (PciDevice->SetupMPS, MpsValue);
    PciDevice->SetupMPS = MIN (PciDevice->SetupMPS, MpsValue);

    if (MpsValue != PciFeaturesConfigurationTable->Max_Payload_Size) {
      PciFeaturesConfigurationTable->Max_Payload_Size = MpsValue;
    }
  }

  DEBUG (( DEBUG_INFO,
      "MPS: %d [DevCap:%d],",
      PciDevice->SetupMPS, PciDeviceCap.Bits.MaxPayloadSize
  ));
  return EFI_SUCCESS;
}

/**
  Overrides the PCI Device Control register MaxPayloadSize register field; if
  the hardware value is different than the intended value.

  @param  PciDevice             A pointer to the PCI_IO_DEVICE instance.

  @retval EFI_SUCCESS           The data was read from or written to the PCI device.
  @retval EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
                                valid for the PCI configuration header of the PCI controller.
  @retval EFI_INVALID_PARAMETER Buffer is NULL or Width is invalid.

**/
EFI_STATUS
OverrideMaxPayloadSize (
  IN PCI_IO_DEVICE          *PciDevice
  )
{
  PCI_REG_PCIE_DEVICE_CONTROL PcieDev;
  UINT32                      Offset;
  EFI_STATUS                  Status;
  EFI_TPL                     OldTpl;

  PcieDev.Uint16 = 0;
  Offset = PciDevice->PciExpressCapabilityOffset +
               OFFSET_OF (PCI_CAPABILITY_PCIEXP, DeviceControl);
  Status = PciDevice->PciIo.Pci.Read (
                                  &PciDevice->PciIo,
                                  EfiPciIoWidthUint16,
                                  Offset,
                                  1,
                                  &PcieDev.Uint16
                                );
  if (EFI_ERROR(Status)){
    DEBUG (( DEBUG_ERROR, "Unexpected DeviceControl register (0x%x) read error!",
        Offset
    ));
    return Status;
  }
  if (PcieDev.Bits.MaxPayloadSize != PciDevice->SetupMPS) {
    PcieDev.Bits.MaxPayloadSize = PciDevice->SetupMPS;
    DEBUG (( DEBUG_INFO, "MPS=%d,", PciDevice->SetupMPS));

    //
    // Raise TPL to high level to disable timer interrupt while the write operation completes
    //
    OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);

    Status = PciDevice->PciIo.Pci.Write (
                                    &PciDevice->PciIo,
                                    EfiPciIoWidthUint16,
                                    Offset,
                                    1,
                                    &PcieDev.Uint16
                                  );
    //
    // Restore TPL to its original level
    //
    gBS->RestoreTPL (OldTpl);

    if (!EFI_ERROR(Status)) {
      PciDevice->PciExpStruct.DeviceControl.Uint16 = PcieDev.Uint16;
    } else {
      DEBUG (( DEBUG_ERROR, "Unexpected DeviceControl register (0x%x) write error!",
          Offset
      ));
    }
  } else {
    DEBUG (( DEBUG_INFO, "No write of MPS=%d,", PciDevice->SetupMPS));
  }

  return Status;
}

/**
  helper routine to dump the PCIe Device Port Type
**/
VOID
DumpDevicePortType (
  IN  UINT8   DevicePortType
  )
{
  switch (DevicePortType){
    case PCIE_DEVICE_PORT_TYPE_PCIE_ENDPOINT:
      DEBUG (( DEBUG_INFO, "PCIe endpoint found\n"));
      break;
    case PCIE_DEVICE_PORT_TYPE_LEGACY_PCIE_ENDPOINT:
      DEBUG (( DEBUG_INFO, "legacy PCI endpoint found\n"));
      break;
    case PCIE_DEVICE_PORT_TYPE_ROOT_PORT:
      DEBUG (( DEBUG_INFO, "PCIe Root Port found\n"));
      break;
    case PCIE_DEVICE_PORT_TYPE_UPSTREAM_PORT:
      DEBUG (( DEBUG_INFO, "PCI switch upstream port found\n"));
      break;
    case PCIE_DEVICE_PORT_TYPE_DOWNSTREAM_PORT:
      DEBUG (( DEBUG_INFO, "PCI switch downstream port found\n"));
      break;
    case PCIE_DEVICE_PORT_TYPE_PCIE_TO_PCI_BRIDGE:
      DEBUG (( DEBUG_INFO, "PCIe-PCI bridge found\n"));
      break;
    case PCIE_DEVICE_PORT_TYPE_PCI_TO_PCIE_BRIDGE:
      DEBUG (( DEBUG_INFO, "PCI-PCIe bridge found\n"));
      break;
    case PCIE_DEVICE_PORT_TYPE_ROOT_COMPLEX_INTEGRATED_ENDPOINT:
      DEBUG (( DEBUG_INFO, "RCiEP found\n"));
      break;
    case PCIE_DEVICE_PORT_TYPE_ROOT_COMPLEX_EVENT_COLLECTOR:
      DEBUG (( DEBUG_INFO, "RC Event Collector found\n"));
      break;
  }
}

/**
   Process each PCI device as per the pltaform and device-specific policy.

  @param RootBridge             A pointer to the PCI_IO_DEVICE.

  @retval EFI_SUCCESS           processing each PCI feature as per policy defined
                                was successful.
 **/
EFI_STATUS
SetupDevicePciFeatures (
  IN  PCI_IO_DEVICE                   *PciDevice,
  IN  PCI_FEATURE_CONFIGURATION_PHASE PciConfigPhase
  )
{
  EFI_STATUS                              Status;
  PCI_REG_PCIE_CAPABILITY                 PcieCap;
  OTHER_PCI_FEATURES_CONFIGURATION_TABLE  *OtherPciFeaturesConfigTable;

  PcieCap.Uint16 = PciDevice->PciExpStruct.Capability.Uint16;
  DumpDevicePortType ((UINT8)PcieCap.Bits.DevicePortType);

  OtherPciFeaturesConfigTable = NULL;
  Status = GetPciFeaturesConfigurationTable (PciDevice, &OtherPciFeaturesConfigTable);
  if (EFI_ERROR( Status)) {
    DEBUG ((
       DEBUG_WARN, "[Cfg group: 0 {error in dev path}]"
    ));
  } else if (OtherPciFeaturesConfigTable == NULL) {
    DEBUG ((
        DEBUG_INFO, "[Cfg group: 0]"
    ));
  } else {
    DEBUG ((
        DEBUG_INFO, "[Cfg group: %d]",
        OtherPciFeaturesConfigTable->ID
    ));
  }

  if (PciConfigPhase == PciFeatureGetDevicePolicy) {
    Status = GetPciDevicePlatformPolicy (PciDevice);
    if (EFI_ERROR(Status)) {
      DEBUG ((
          DEBUG_ERROR, "Error in obtaining PCI device policy!!!\n"
      ));
    }
  }

  DEBUG ((DEBUG_INFO, "["));
  //
  // process the PCI device Max_Payload_Size feature
  //
  if (SetupMaxPayloadSize ()) {
    Status = ProcessMaxPayloadSize (
              PciDevice,
              PciConfigPhase,
              OtherPciFeaturesConfigTable
              );
  }
  DEBUG ((DEBUG_INFO, "]\n"));
  return Status;
}

/**
  Traverse all the nodes from the root bridge or PCI-PCI bridge instance, to
  configure the PCI features as per the device-specific platform policy, and
  as per the device capability, as applicable.

  @param RootBridge             A pointer to the PCI_IO_DEVICE.

  @retval EFI_SUCCESS           Traversing all the nodes of the root bridge
                                instances were successfull.
**/
EFI_STATUS
SetupPciFeatures (
  IN  PCI_IO_DEVICE                   *RootBridge,
  IN  PCI_FEATURE_CONFIGURATION_PHASE PciConfigPhase
  )
{
  EFI_STATUS            Status;
  LIST_ENTRY            *Link;
  PCI_IO_DEVICE         *Device;

  for ( Link = RootBridge->ChildList.ForwardLink
      ; Link != &RootBridge->ChildList
      ; Link = Link->ForwardLink
  ) {
    Device = PCI_IO_DEVICE_FROM_LINK (Link);
    if (!DeviceExist (Device)) {
      DEBUG ((
          DEBUG_ERROR, "::Device [%02x|%02x|%02x] - does not exist!!!\n",
          Device->BusNumber, Device->DeviceNumber, Device->FunctionNumber
      ));
      continue;
    }
    if (IS_PCI_BRIDGE (&Device->Pci)) {
      DEBUG ((
          DEBUG_INFO, "::Bridge [%02x|%02x|%02x] -",
          Device->BusNumber, Device->DeviceNumber, Device->FunctionNumber
      ));
      if (Device->IsPciExp) {
        Status = SetupDevicePciFeatures (Device, PciConfigPhase);
      } else {
        DEBUG (( DEBUG_INFO, "Not a PCIe capable device!\n"));
        //
        // PCI Bridge which does not have PCI Express Capability structure
        // cannot process this kind of PCI Bridge device
        //

      }

      SetupPciFeatures (Device, PciConfigPhase);
    } else {
      DEBUG ((
          DEBUG_INFO, "::Device [%02x|%02x|%02x] -",
          Device->BusNumber, Device->DeviceNumber, Device->FunctionNumber
      ));
      if (Device->IsPciExp) {

        Status = SetupDevicePciFeatures (Device, PciConfigPhase);
      } else {
        DEBUG (( DEBUG_INFO, "Not a PCIe capable device!\n"));
        //
        // PCI Device which does not have PCI Express Capability structure
        // cannot process this kind of PCI device
        //
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Program the PCI device, to override the PCI features as per the policy,
  resolved from previous traverse.

  @param RootBridge             A pointer to the PCI_IO_DEVICE.

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
ProgramDevicePciFeatures (
  IN PCI_IO_DEVICE          *PciDevice
  )
{
  EFI_STATUS           Status = EFI_SUCCESS;

  if (SetupMaxPayloadSize ()) {
    Status = OverrideMaxPayloadSize (PciDevice);
  }
  DEBUG (( DEBUG_INFO, "\n"));
  return Status;
}

/**
  Program all the nodes of the specified root bridge or PCI-PCI Bridge, to
  override the PCI features.

  @param RootBridge             A pointer to the PCI_IO_DEVICE.

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
ProgramPciFeatures (
  IN PCI_IO_DEVICE          *RootBridge
  )
{
  EFI_STATUS           Status;
  LIST_ENTRY           *Link;
  PCI_IO_DEVICE        *Device;

  for ( Link = RootBridge->ChildList.ForwardLink
      ; Link != &RootBridge->ChildList
      ; Link = Link->ForwardLink
  ) {
    Device = PCI_IO_DEVICE_FROM_LINK (Link);
    if (!DeviceExist (Device)) {
      DEBUG ((
          DEBUG_ERROR, "::Device [%02x|%02x|%02x] - does not exist!!!\n",
          Device->BusNumber, Device->DeviceNumber, Device->FunctionNumber
      ));
      continue;
    }
    if (IS_PCI_BRIDGE (&Device->Pci)) {
      DEBUG ((
          DEBUG_INFO, "::Bridge [%02x|%02x|%02x] -",
          Device->BusNumber, Device->DeviceNumber, Device->FunctionNumber
      ));
      if (Device->IsPciExp) {
        DEBUG (( DEBUG_INFO, "ready to override!\n"));

        Status = ProgramDevicePciFeatures (Device);
      } else {
        DEBUG (( DEBUG_INFO, "skipped!\n"));
        //
        // PCI Bridge which does not have PCI Express Capability structure
        // cannot process this kind of PCI Bridge device
        //
      }

      Status = ProgramPciFeatures (Device);
    } else {
      DEBUG ((
          DEBUG_INFO, "::Device [%02x|%02x|%02x] -",
          Device->BusNumber, Device->DeviceNumber, Device->FunctionNumber
      ));
      if (Device->IsPciExp) {
        DEBUG (( DEBUG_INFO, "ready to override!\n"));

        Status = ProgramDevicePciFeatures (Device);
      } else {
        DEBUG (( DEBUG_INFO, "skipped!\n"));
        //
        // PCI Device which does not have PCI Express Capability structure
        // cannot process this kind of PCI device
        //
      }
    }
  }

  return Status;
}

/**
  Create and add a node of type PRIMARY_ROOT_PORT_NODE in the list for the primary
  Root Port so that all its child PCI devices can be identified against the PCI
  features configuration table group ID, of type OTHER_PCI_FEATURES_CONFIGURATION_TABLE.

  @param BridgePort    A pointer to the PCI_IO_DEVICE
  @param PortNumber    A UINTN value to identify the PCI feature configuration
                       table group

  @retval EFI_SUCCESS           success in adding a node of PRIMARY_ROOT_PORT_NODE
                                to the list
          EFI_OUT_OF_RESOURCES  unable to get memory for creating the node
**/
EFI_STATUS
AddPrimaryRootPortNode (
  IN  PCI_IO_DEVICE           *BridgePort,
  IN  UINTN                   PortNumber
  )
{
  PRIMARY_ROOT_PORT_NODE                  *RootPortNode = NULL;
  OTHER_PCI_FEATURES_CONFIGURATION_TABLE  *PciConfigTable = NULL;

  RootPortNode = AllocateZeroPool (sizeof (PRIMARY_ROOT_PORT_NODE));
  if (RootPortNode == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  RootPortNode->Signature                           = PCI_ROOT_PORT_SIGNATURE;
  RootPortNode->RootPortDevicePath                  = BridgePort->DevicePath;
  PciConfigTable = AllocateZeroPool (
                     sizeof (OTHER_PCI_FEATURES_CONFIGURATION_TABLE)
                    );
  if (PciConfigTable) {
    PciConfigTable->ID                          = PortNumber;
    PciConfigTable->Max_Payload_Size            = PCIE_MAX_PAYLOAD_SIZE_4096B;
  }
  RootPortNode->OtherPciFeaturesConfigurationTable  = PciConfigTable;

  if (mPrimaryRootPortList != NULL) {
    InsertTailList (&mPrimaryRootPortList->NeighborRootPort, &RootPortNode->NeighborRootPort);
  } else {
    InitializeListHead (&RootPortNode->NeighborRootPort);
    mPrimaryRootPortList = RootPortNode;
  }

  if (PciConfigTable == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}

/**
  Scan all the nodes of the RootBridge to identify and create a separate list
  of all primary physical PCI root ports and link each with its own instance of
  the PCI Feature Configuration Table.

  @param  RootBridge    A pointer to the PCI_IO_DEVICE of the PCI Root Bridge

  @retval EFI_OUT_OF_RESOURCES  unable to allocate buffer to store PCI feature
                                configuration table for all the physical PCI root
                                ports given
          EFI_NOT_FOUND         No PCI Bridge device found
          EFI_SUCCESS           PCI Feature COnfiguration table created for all
                                the PCI Rooot ports found
          EFI_INVALID_PARAMETER invalid parameter passed to the routine which
                                creates the PCI controller node for the primary
                                Root post list
**/
EFI_STATUS
RecordPciRootPortBridges (
  IN  PCI_IO_DEVICE           *RootBridge
  )
{
  EFI_STATUS              Status = EFI_NOT_FOUND;
  LIST_ENTRY              *Link;
  PCI_IO_DEVICE           *Device;
  UINTN                   NumberOfRootPorts;

  DEBUG ((
      DEBUG_INFO, "<<********** RecordPciRootPortBridges -start *************>>\n"
  ));
  NumberOfRootPorts = 0;
  for ( Link = RootBridge->ChildList.ForwardLink
      ; Link != &RootBridge->ChildList
      ; Link = Link->ForwardLink
  ) {
    Device = PCI_IO_DEVICE_FROM_LINK (Link);
    if (!DeviceExist (Device)) {
      continue;
    }
    if (IS_PCI_BRIDGE (&Device->Pci)) {
      NumberOfRootPorts++;
      DEBUG ((
          DEBUG_INFO, "#%d ::Bridge [%02x|%02x|%02x]",
          NumberOfRootPorts, Device->BusNumber, Device->DeviceNumber, Device->FunctionNumber
      ));
      //
      // create a primary root port list if that port is connected
      //
      if (!IsListEmpty (&Device->ChildList)) {
        DEBUG ((
            DEBUG_INFO, "- has downstream device!\n"
        ));
        Status = AddPrimaryRootPortNode (Device, NumberOfRootPorts);
        if (EFI_ERROR (Status)) {
          DEBUG ((
              DEBUG_ERROR, "PCI configuration table allocation failure for #%d ::Bridge [%02x|%02x|%02x]\n",
              NumberOfRootPorts, Device->BusNumber, Device->DeviceNumber, Device->FunctionNumber
          ));
        }
      } else {
        DEBUG ((
            DEBUG_INFO, "- no downstream device!\n"
        ));
      }
    }
  }
  DEBUG ((
      DEBUG_INFO, "<<********** RecordPciRootPortBridges - end **********>>\n"
  ));
  return Status;
}

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
  IN PCI_IO_DEVICE          *RootBridge
  )
{
  EFI_STATUS            Status;
  CHAR16                *Str;
  UINTN                 OtherPciFeatureConfigPhase;

  //
  // check on PCI features configuration is complete and re-enumeration is required
  //
  if (!CheckPciFeaturesConfigurationRequired (RootBridge)) {
    return EFI_ALREADY_STARTED;
  }

  Str = ConvertDevicePathToText (
          DevicePathFromHandle (RootBridge->Handle),
          FALSE,
          FALSE
        );
  DEBUG ((DEBUG_INFO, "Enumerating PCI features for Root Bridge %s\n", Str != NULL ? Str : L""));

  for ( OtherPciFeatureConfigPhase = PciFeatureRootBridgeScan
      ; OtherPciFeatureConfigPhase <= PciFeatureConfigurationComplete
      ; OtherPciFeatureConfigPhase++
      ) {
    switch (OtherPciFeatureConfigPhase){
      case  PciFeatureRootBridgeScan:
        SetupPciFeaturesConfigurationDefaults ();
        //
        //first scan the entire root bridge heirarchy for the primary PCI root ports
        //
        RecordPciRootPortBridges (RootBridge);
        break;

      case  PciFeatureGetDevicePolicy:
      case  PciFeatureSetupPhase:
        DEBUG ((
            DEBUG_INFO, "<<********** SetupPciFeatures - start **********>>\n"
        ));
        //
        // enumerate the other PCI features
        //
        Status = SetupPciFeatures (RootBridge, OtherPciFeatureConfigPhase);

        DEBUG ((
            DEBUG_INFO, "<<********** SetupPciFeatures - end **********>>\n"
        ));
        break;

      case  PciFeatureConfigurationPhase:
        //
        // override the PCI features as per enumeration phase
        //
        DEBUG ((DEBUG_INFO, "PCI features override for Root Bridge %s\n", Str != NULL ? Str : L""));
        DEBUG ((
            DEBUG_INFO, "<<********** ProgramPciFeatures - start **********>>\n"
        ));
        Status = ProgramPciFeatures (RootBridge);
        DEBUG ((
            DEBUG_INFO, "<<********** ProgramPciFeatures - end **********>>\n"
        ));
        break;

      case  PciFeatureConfigurationComplete:
        //
        // clean up the temporary resource nodes created for this root bridge
        //
        DestroyPrimaryRootPortNodes ();
    }
  }

  if (Str != NULL) {
    FreePool (Str);
  }
  //
  // mark this root bridge as PCI features configuration complete, and no new
  // enumeration is required
  //
  AddRootBridgeInPciFeaturesConfigCompletionList (RootBridge, FALSE);
  return Status;
}

/**
  This routine is invoked from the Stop () interface for the EFI handle of the
  RootBridge. Free up its node of type PCI_FEATURE_CONFIGURATION_COMPLETION_LIST.

  @param  RootBridge      A pointer to the PCI_IO_DEVICE
**/
VOID
DestroyRootBridgePciFeaturesConfigCompletionList (
  IN PCI_IO_DEVICE          *RootBridge
  )
{
  LIST_ENTRY                                  *Link;
  PCI_FEATURE_CONFIGURATION_COMPLETION_LIST   *Temp;

  if (mPciFeaturesConfigurationCompletionList) {
    Link = &mPciFeaturesConfigurationCompletionList->RootBridgeLink;

    do {
      Temp = PCI_FEATURE_CONFIGURATION_COMPLETION_LIST_FROM_LINK (Link);
      if (Temp->RootBridgeHandle == RootBridge->Handle) {
        RemoveEntryList (Link);
        FreePool (Temp);
        return;
      }
      Link = Link->ForwardLink;
    } while (Link != &mPciFeaturesConfigurationCompletionList->RootBridgeLink);
  }
  //
  // not found on the PCI feature configuration completion list, return
  //
  return;
}
