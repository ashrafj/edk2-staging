/** @file
  PCI standard feature support functions implementation for PCI Bus module..

Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PciBus.h"
#include "PcieFeatureSupport.h"


/**
  Scan the devices to finalize the MaxPayloadSize settings of each device.

  @param PciDevice              A pointer to the PCI_IO_DEVICE.
  @param Level                  The level of the PCI device in the heirarchy.
                                Level of root ports is 0.
  @param Context                Pointer to feature specific context.

  @retval EFI_SUCCESS           setup of PCI feature MaxPayloadSize is successful.
**/
EFI_STATUS
MaxPayloadSizeScan (
  IN PCI_IO_DEVICE *PciDevice,
  IN UINTN         Level,
  IN VOID          **Context
  )
{
  UINT8                           *MaxPayloadSize;
  PCI_REG_PCIE_DEVICE_CAPABILITY  DeviceCapability;

  DEBUG ((
    DEBUG_INFO, "  %a [%02d|%02d|%02d]: Capability = %x\n",
    __FUNCTION__, PciDevice->BusNumber, PciDevice->DeviceNumber, PciDevice->FunctionNumber,
    PciDevice->PciExpressCapability.DeviceCapability.Bits.MaxPayloadSize
    ));
  DeviceCapability.Uint32 = PciDevice->PciExpressCapability.DeviceCapability.Uint32;

  if ((PciDevice->DeviceState.MaxPayloadSize != EFI_PCI_EXPRESS_DEVICE_POLICY_AUTO) &&
      (PciDevice->DeviceState.MaxPayloadSize != EFI_PCI_EXPRESS_DEVICE_POLICY_NOT_APPLICABLE)) {
    DeviceCapability.Bits.MaxPayloadSize =
      MIN (PciDevice->DeviceState.MaxPayloadSize, DeviceCapability.Bits.MaxPayloadSize);
  }

  MaxPayloadSize = *Context;
  if (MaxPayloadSize == NULL) {
    //
    // Initialize the Context
    //
    MaxPayloadSize  = AllocatePool (sizeof (*MaxPayloadSize));
    *MaxPayloadSize = (UINT8) DeviceCapability.Bits.MaxPayloadSize;
    *Context        = MaxPayloadSize;
  } else {
    //
    // Set the Context to the minimum Max Payload Size in the heirarchy.
    //
    *MaxPayloadSize = MIN (*MaxPayloadSize, (UINT8) DeviceCapability.Bits.MaxPayloadSize);
  }
  return EFI_SUCCESS;
}

/**
  Program the PCIe Device Control register Max. Payload Size field per platform policy.

  @param PciDevice              A pointer to the PCI_IO_DEVICE instance.
  @param Level                  The level of the PCI device in the heirarchy.
                                Level of root ports is 0.
  @param Context                Pointer to feature specific context.

  @retval EFI_SUCCESS           The data was read from or written to the PCI device.
  @retval EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
                                valid for the PCI configuration header of the PCI controller.
  @retval EFI_INVALID_PARAMETER Buffer is NULL or Width is invalid.
**/
EFI_STATUS
MaxPayloadSizeProgram (
  IN PCI_IO_DEVICE *PciDevice,
  IN UINTN         Level,
  IN VOID          **Context
  )
{
  UINT8 *MaxPayloadSize;

  ASSERT (Context != NULL);
  ASSERT (*Context != NULL);

  if (PciDevice->DeviceState.MaxPayloadSize == EFI_PCI_EXPRESS_DEVICE_POLICY_NOT_APPLICABLE) {
    //
    // NOT_APPLICABLE means platform requests PciBus doesn't change the setting.
    // But the capability of this device is still honored when calculating the aligned value.
    //
    return EFI_SUCCESS;
  }

  MaxPayloadSize                        = (UINT8 *) *Context;
  PciDevice->DeviceState.MaxPayloadSize = *MaxPayloadSize;

  if (PciDevice->PciExpressCapability.DeviceControl.Bits.MaxPayloadSize != PciDevice->DeviceState.MaxPayloadSize) {
    DEBUG ((
      DEBUG_INFO, "  %a [%02d|%02d|%02d]: %x -> %x\n",
      __FUNCTION__, PciDevice->BusNumber, PciDevice->DeviceNumber, PciDevice->FunctionNumber,
      PciDevice->PciExpressCapability.DeviceControl.Bits.MaxPayloadSize,
      PciDevice->DeviceState.MaxPayloadSize
      ));
    PciDevice->PciExpressCapability.DeviceControl.Bits.MaxPayloadSize = PciDevice->DeviceState.MaxPayloadSize;

    return PciDevice->PciIo.Pci.Write (
                                  &PciDevice->PciIo,
                                  EfiPciIoWidthUint16,
                                  PciDevice->PciExpressCapabilityOffset
                                  + OFFSET_OF (PCI_CAPABILITY_PCIEXP, DeviceControl),
                                  1,
                                  &PciDevice->PciExpressCapability.DeviceControl.Uint16
                                  );
  }
  return EFI_SUCCESS;
}

/**
  Program the PCIe Device Control register Max. Read Request Size field per platform policy.

  @param PciDevice              A pointer to the PCI_IO_DEVICE instance.
  @param Level                  The level of the PCI device in the heirarchy.
                                Level of root ports is 0.
  @param Context                Pointer to feature specific context.

  @retval EFI_SUCCESS           The data was read from or written to the PCI device.
  @retval EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
                                valid for the PCI configuration header of the PCI controller.
  @retval EFI_INVALID_PARAMETER Buffer is NULL or Width is invalid.
**/
EFI_STATUS
MaxReadRequestSizeProgram (
  IN PCI_IO_DEVICE *PciDevice,
  IN UINTN         Level,
  IN VOID          **Context
  )
{
  ASSERT (*Context == NULL);

  if (PciDevice->DeviceState.MaxReadRequestSize == EFI_PCI_EXPRESS_DEVICE_POLICY_NOT_APPLICABLE) {
    return EFI_SUCCESS;
  }
  if (PciDevice->DeviceState.MaxReadRequestSize == EFI_PCI_EXPRESS_DEVICE_POLICY_AUTO) {
    PciDevice->DeviceState.MaxReadRequestSize = (UINT8) PciDevice->PciExpressCapability.DeviceControl.Bits.MaxPayloadSize;
  }

  if (PciDevice->PciExpressCapability.DeviceControl.Bits.MaxReadRequestSize != PciDevice->DeviceState.MaxReadRequestSize) {
    DEBUG ((
      DEBUG_INFO, "  %a [%02d|%02d|%02d]: %x -> %x\n",
      __FUNCTION__, PciDevice->BusNumber, PciDevice->DeviceNumber, PciDevice->FunctionNumber,
      PciDevice->PciExpressCapability.DeviceControl.Bits.MaxReadRequestSize,
      PciDevice->DeviceState.MaxReadRequestSize
      ));
    PciDevice->PciExpressCapability.DeviceControl.Bits.MaxReadRequestSize = PciDevice->DeviceState.MaxReadRequestSize;

    return PciDevice->PciIo.Pci.Write (
                                  &PciDevice->PciIo,
                                  EfiPciIoWidthUint16,
                                  PciDevice->PciExpressCapabilityOffset
                                  + OFFSET_OF (PCI_CAPABILITY_PCIEXP, DeviceControl),
                                  1,
                                  &PciDevice->PciExpressCapability.DeviceControl.Uint16
                                  );
  }
  return EFI_SUCCESS;
}


/**
  Program the PCIe Device Control register Relaxed Ordering field per platform policy.

  @param PciDevice              A pointer to the PCI_IO_DEVICE instance.
  @param Level                  The level of the PCI device in the heirarchy.
                                Level of root ports is 0.
  @param Context                Pointer to feature specific context.

  @retval EFI_SUCCESS           The data was read from or written to the PCI device.
  @retval EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
                                valid for the PCI configuration header of the PCI controller.
  @retval EFI_INVALID_PARAMETER Buffer is NULL or Width is invalid.
**/
EFI_STATUS
RelaxedOrderingProgram (
  IN PCI_IO_DEVICE *PciDevice,
  IN UINTN         Level,
  IN VOID          **Context
  )
{
  ASSERT (*Context == NULL);

  if (PciDevice->DeviceState.RelaxedOrdering == EFI_PCI_EXPRESS_DEVICE_POLICY_NOT_APPLICABLE ||
      PciDevice->DeviceState.RelaxedOrdering == EFI_PCI_EXPRESS_DEVICE_POLICY_AUTO) {
    return EFI_SUCCESS;
  }

  if (PciDevice->PciExpressCapability.DeviceControl.Bits.RelaxedOrdering != PciDevice->DeviceState.RelaxedOrdering) {
    DEBUG ((
      DEBUG_INFO, "  %a [%02d|%02d|%02d]: %x -> %x\n",
      __FUNCTION__, PciDevice->BusNumber, PciDevice->DeviceNumber, PciDevice->FunctionNumber,
      PciDevice->PciExpressCapability.DeviceControl.Bits.RelaxedOrdering,
      PciDevice->DeviceState.RelaxedOrdering
      ));
    PciDevice->PciExpressCapability.DeviceControl.Bits.RelaxedOrdering = PciDevice->DeviceState.RelaxedOrdering;

    return PciDevice->PciIo.Pci.Write (
                                  &PciDevice->PciIo,
                                  EfiPciIoWidthUint16,
                                  PciDevice->PciExpressCapabilityOffset
                                  + OFFSET_OF (PCI_CAPABILITY_PCIEXP, DeviceControl),
                                  1,
                                  &PciDevice->PciExpressCapability.DeviceControl.Uint16
                                  );
  }
  return EFI_SUCCESS;
}

/**
  Overrides the PCI Device Control register No-Snoop register field; if
  the hardware value is different than the intended value.

  @param PciDevice              A pointer to the PCI_IO_DEVICE instance.
  @param Level                  The level of the PCI device in the heirarchy.
                                Level of root ports is 0.
  @param Context                Pointer to feature specific context.

  @retval EFI_SUCCESS           The data was read from or written to the PCI device.
  @retval EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
                                valid for the PCI configuration header of the PCI controller.
  @retval EFI_INVALID_PARAMETER Buffer is NULL or Width is invalid.

**/
EFI_STATUS
NoSnoopProgram (
  IN PCI_IO_DEVICE *PciDevice,
  IN UINTN         Level,
  IN VOID          **Context
  )
{
  ASSERT (*Context == NULL);

  if (PciDevice->DeviceState.NoSnoop == EFI_PCI_EXPRESS_DEVICE_POLICY_NOT_APPLICABLE ||
      PciDevice->DeviceState.NoSnoop == EFI_PCI_EXPRESS_DEVICE_POLICY_AUTO) {
    return EFI_SUCCESS;
  }


  if (PciDevice->PciExpressCapability.DeviceControl.Bits.NoSnoop != PciDevice->DeviceState.NoSnoop) {
    DEBUG ((
      DEBUG_INFO, "  %a [%02d|%02d|%02d]: %x -> %x\n",
      __FUNCTION__, PciDevice->BusNumber, PciDevice->DeviceNumber, PciDevice->FunctionNumber,
      PciDevice->PciExpressCapability.DeviceControl.Bits.NoSnoop,
      PciDevice->DeviceState.NoSnoop
      ));
    PciDevice->PciExpressCapability.DeviceControl.Bits.NoSnoop = PciDevice->DeviceState.NoSnoop;

    return PciDevice->PciIo.Pci.Write (
                                  &PciDevice->PciIo,
                                  EfiPciIoWidthUint16,
                                  PciDevice->PciExpressCapabilityOffset
                                  + OFFSET_OF (PCI_CAPABILITY_PCIEXP, DeviceControl),
                                  1,
                                  &PciDevice->PciExpressCapability.DeviceControl.Uint16
                                  );
  }
  return EFI_SUCCESS;
}

/**
  Program PCIe feature Completion Timeout per the device-specific platform policy.

  @param PciIoDevice            A pointer to the PCI_IO_DEVICE.
  @param Level                  The level of the PCI device in the heirarchy.
                                Level of root ports is 0.
  @param Context                Pointer to feature specific context.

  @retval EFI_SUCCESS           The feature is initialized successfully.
  @retval EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
                                valid for the PCI configuration header of the PCI controller.
  @retval EFI_INVALID_PARAMETER Buffer is NULL or Width is invalid.
**/
EFI_STATUS
CompletionTimeoutProgram (
  IN PCI_IO_DEVICE *PciIoDevice,
  IN UINTN         Level,
  IN VOID          **Context
  )
{
  PCI_REG_PCIE_DEVICE_CONTROL2    DevicePolicy;
  UINTN                           RangeIndex;
  UINT8                           SubRanges;

  if (PciIoDevice->DeviceState.CompletionTimeout == EFI_PCI_EXPRESS_DEVICE_POLICY_NOT_APPLICABLE ||
      PciIoDevice->DeviceState.CompletionTimeout == EFI_PCI_EXPRESS_DEVICE_POLICY_AUTO) {
    return EFI_SUCCESS;
  }

  //
  // Interpret the policy value as BIT[0:4] in Device Control 2 Register
  //
  DevicePolicy.Uint16 = (UINT16) PciIoDevice->DeviceState.CompletionTimeout;

  //
  // Ignore when device doesn't support to disable Completion Timeout while the policy requests.
  //
  if (PciIoDevice->PciExpressCapability.DeviceCapability2.Bits.CompletionTimeoutDisable == 0 &&
      DevicePolicy.Bits.CompletionTimeoutDisable == 1) {
    return EFI_INVALID_PARAMETER;
  }

  if (DevicePolicy.Bits.CompletionTimeoutValue != 0) {
    //
    // Ignore when the policy requests to use a range that's not supported by the device.
    // RangeIndex is 0 ~ 3 for Range A ~ D.
    //
    RangeIndex = DevicePolicy.Bits.CompletionTimeoutValue >> 2;
    if ((PciIoDevice->PciExpressCapability.DeviceCapability2.Bits.CompletionTimeoutRanges & (1 < RangeIndex)) == 0) {
      return EFI_INVALID_PARAMETER;
    }

    //
    // Ignore when the policy doesn't request one and only one sub-range for a certain range.
    //
    SubRanges = (UINT8) (DevicePolicy.Bits.CompletionTimeoutValue & (BIT0 | BIT1));
    if (SubRanges != BIT0 && SubRanges != BIT1) {
      return EFI_INVALID_PARAMETER;
    }
  }

  if ((PciIoDevice->PciExpressCapability.DeviceControl2.Bits.CompletionTimeoutDisable
       != DevicePolicy.Bits.CompletionTimeoutDisable) ||
      (PciIoDevice->PciExpressCapability.DeviceControl2.Bits.CompletionTimeoutValue
       != DevicePolicy.Bits.CompletionTimeoutValue)) {
    DEBUG ((
      DEBUG_INFO, "  %a [%02d|%02d|%02d]: Disable = %x -> %x, Timeout = %x -> %x.\n",
      __FUNCTION__, PciIoDevice->BusNumber, PciIoDevice->DeviceNumber, PciIoDevice->FunctionNumber,
      PciIoDevice->PciExpressCapability.DeviceControl2.Bits.CompletionTimeoutDisable,
      DevicePolicy.Bits.CompletionTimeoutDisable,
      PciIoDevice->PciExpressCapability.DeviceControl2.Bits.CompletionTimeoutValue,
      DevicePolicy.Bits.CompletionTimeoutValue
      ));
    PciIoDevice->PciExpressCapability.DeviceControl2.Bits.CompletionTimeoutDisable
                      = DevicePolicy.Bits.CompletionTimeoutDisable;
    PciIoDevice->PciExpressCapability.DeviceControl2.Bits.CompletionTimeoutValue
                      = DevicePolicy.Bits.CompletionTimeoutValue;

    return PciIoDevice->PciIo.Pci.Write (
                                  &PciIoDevice->PciIo,
                                  EfiPciIoWidthUint16,
                                  PciIoDevice->PciExpressCapabilityOffset
                                  + OFFSET_OF (PCI_CAPABILITY_PCIEXP, DeviceControl2),
                                  1,
                                  &PciIoDevice->PciExpressCapability.DeviceControl2.Uint16
                                  );
  }

  return EFI_SUCCESS;
}

/**
  disable LTR policy of the device. If bridge device than disable LTR policy of
  all its child devices.

  @param Bridge     A pointer to the PCI_IO_DEVICE
**/
STATIC
VOID
DisableLtrPolicy (
  IN  PCI_IO_DEVICE *Bridge
  )
{
  LIST_ENTRY            *Link;
  PCI_IO_DEVICE         *PciIoDevice;

  Bridge->DeviceState.Ltr = FALSE;

  for ( Link = GetFirstNode (&Bridge->ChildList)
      ; !IsNull (&Bridge->ChildList, Link)
      ; Link = GetNextNode (&Bridge->ChildList, Link)
  ) {
    PciIoDevice = PCI_IO_DEVICE_FROM_LINK (Link);
    if (PciIoDevice->IsPciExp) {
      DisableLtrPolicy (PciIoDevice);
    }
  }
}

/**
  Update the LTR for each level of devices.

  @param Result     A pointer to the BOOLEAN
  @param Ltr        value TRUE or FALSE.
**/
STATIC
VOID
LtrOr (
  IN OUT  BOOLEAN   *Result,
  IN      BOOLEAN   Ltr
)
{
  ASSERT (Result != NULL);
  ASSERT (*Result == 0xFF || *Result == TRUE || *Result == FALSE);
  if (*Result == 0xFF) {
    //
    // Initialize Result when meeting the first device in the Level.
    //
    *Result = Ltr;
  } else {
    //
    // Save the "OR" result of LTR of all devices in this Level.
    //
    *Result = (*Result || Ltr);
  }
}

/**
  Scan the devices to finalize the LTR settings of each device.

  The scan needs to be done in post-order.

  @param PciIoDevice            A pointer to the PCI_IO_DEVICE.
  @param Level                  The level of the PCI device in the heirarchy.
                                Level of root ports is 0.
  @param Context                Pointer to feature specific context.


  @retval EFI_SUCCESS           setup of PCI feature LTR is successful.
**/
EFI_STATUS
LtrScan (
  IN  PCI_IO_DEVICE *PciIoDevice,
  IN  UINTN         Level,
  IN  VOID          **Context
  )
{
  BOOLEAN           *Ltr;
  ASSERT (Level <= PCI_MAX_BUS);
  //ASSERT (Context != NULL);

  // LTR of a parent (certain bridge) in level N is enabled when any child in
  // level N + 1 enables LTR.
  //
  // Because the devices are enumerated in post-order (children-then-parent),
  // we could allocate one BOOLEAN in Context to save the "OR" result of LTR
  // enable status of all children in a certain level. LTR of parent is set
  // when the "OR" result is TRUE.
  //
  // Because the max level cannot exceed the max PCI bus number 256, allocating
  // a BOOLEAN array of 256 elements should be enough.
  //
  Ltr = (BOOLEAN *) (*Context);
  if (Ltr == NULL) {
    Ltr = AllocatePool (sizeof (BOOLEAN) * (PCI_MAX_BUS + 1));
    SetMem (Ltr, sizeof (BOOLEAN) * (PCI_MAX_BUS + 1), 0xFF);
    *Context = Ltr;
  }

  DEBUG ((
    DEBUG_INFO, "  %a [%02d|%02d|%02d]: Capability = %x.\n",
    __FUNCTION__, PciIoDevice->BusNumber, PciIoDevice->DeviceNumber, PciIoDevice->FunctionNumber,
    PciIoDevice->PciExpressCapability.DeviceCapability2.Bits.LtrMechanism
    ));
  //
  // Disable LTR if the device doesn't support. In case of bridge device disable
  // all its child devices.
  // Even if the platform forces the bridge device to disable the LTR, all its
  // child devices has to be forced to disable LTR because the child cannot send
  // the LTR messages to parent bridge whose LTR is disabled.
  //
  if (!PciIoDevice->PciExpressCapability.DeviceCapability2.Bits.LtrMechanism
      || PciIoDevice->DeviceState.Ltr == FALSE) {
    DisableLtrPolicy (PciIoDevice);
  }

  //
  // If the policy is AUTO or NOT_APPLICABLE for a certain device, only enable LTR
  // when any of its children's LTR is enabled.
  // Note:
  //  It's platform's responsibility to make sure consistent policy is returned.
  //  Inconsistent policy means Bridge's LTR is set to FALSE while child device's LTR is
  //  set to TRUE in platform policy.
  //
  if ((PciIoDevice->DeviceState.Ltr != TRUE) && (PciIoDevice->DeviceState.Ltr != FALSE)) {
    ASSERT (PciIoDevice->DeviceState.Ltr == EFI_PCI_EXPRESS_DEVICE_POLICY_AUTO ||
            PciIoDevice->DeviceState.Ltr == EFI_PCI_EXPRESS_DEVICE_POLICY_NOT_APPLICABLE);

    if ((Level < PCI_MAX_BUS) && (Ltr[Level + 1] != 0xFF)) {
      //
      // LTR of a parent is the "OR" result of LTR of all children.
      //
      PciIoDevice->DeviceState.Ltr = Ltr[Level + 1];
    }
  }

  if ((PciIoDevice->DeviceState.Ltr == TRUE) || (PciIoDevice->DeviceState.Ltr == FALSE)) {
    LtrOr (&Ltr[Level], PciIoDevice->DeviceState.Ltr);
  }

  //
  // Reset the LTR status of Level + 1 because Ltr[Level + 1] will be used by another sub-tree.
  //
  Ltr[Level + 1] = 0xFF;

  return EFI_SUCCESS;
}

/**
  Program the LTR settings of each device.

  The program needs to be done in pre-order per the PCIe spec requirement

  @param PciIoDevice            A pointer to the PCI_IO_DEVICE.
  @param Level                  The level of the PCI device in the heirarchy.
                                Level of root ports is 0.
  @param Context                Pointer to feature specific context.


  @retval EFI_SUCCESS           setup of PCI feature LTR is successful.
  @retval EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
                                valid for the PCI configuration header of the PCI controller.
  @retval EFI_INVALID_PARAMETER Buffer is NULL or Width is invalid.
**/
EFI_STATUS
LtrProgram (
  IN  PCI_IO_DEVICE *PciIoDevice,
  IN  UINTN         Level,
  IN  VOID          **Context
  )
{
  if ((PciIoDevice->DeviceState.Ltr == TRUE) || (PciIoDevice->DeviceState.Ltr == FALSE)) {
    if (PciIoDevice->DeviceState.Ltr != PciIoDevice->PciExpressCapability.DeviceControl2.Bits.LtrMechanism) {

      DEBUG ((
        DEBUG_INFO, "  %a [%02d|%02d|%02d]: %x -> %x.\n",
        __FUNCTION__, PciIoDevice->BusNumber, PciIoDevice->DeviceNumber, PciIoDevice->FunctionNumber,
        PciIoDevice->PciExpressCapability.DeviceControl2.Bits.LtrMechanism,
        PciIoDevice->DeviceState.Ltr
        ));

      return PciIoDevice->PciIo.Pci.Write (
                                    &PciIoDevice->PciIo,
                                    EfiPciIoWidthUint16,
                                    PciIoDevice->PciExpressCapabilityOffset
                                    + OFFSET_OF (PCI_CAPABILITY_PCIEXP, DeviceControl2),
                                    1,
                                    &PciIoDevice->PciExpressCapability.DeviceControl2.Uint16
                                    );
    }
  }

  return EFI_SUCCESS;
}

/**
  Program AtomicOp.

  @param PciIoDevice            A pointer to the PCI_IO_DEVICE.
  @param Level                  The level of the PCI device in the heirarchy.
                                Level of root ports is 0.
  @param Context                Pointer to feature specific context.

  @retval EFI_SUCCESS           setup of PCI feature AtomicOp is successful.
  @retval EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
                                valid for the PCI configuration header of the PCI controller.
  @retval EFI_INVALID_PARAMETER Buffer is NULL or Width is invalid.
**/
EFI_STATUS
AtomicOpProgram (
  IN  PCI_IO_DEVICE *PciIoDevice,
  IN  UINTN         Level,
  IN  VOID          **Context
  )
{
  if (PciIoDevice->DeviceState.AtomicOp == EFI_PCI_EXPRESS_DEVICE_POLICY_AUTO ||
      PciIoDevice->DeviceState.AtomicOp == EFI_PCI_EXPRESS_DEVICE_POLICY_NOT_APPLICABLE) {
    return EFI_SUCCESS;
  }

  //
  // BIT0 of the policy value is for AtomicOp Requester Enable (BIT6)
  // BIT1 of the policy value is for AtomicOp Egress Blocking (BIT7)
  //
  if ((PciIoDevice->DeviceState.AtomicOp >> 2) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (!PciIoDevice->PciExpressCapability.DeviceCapability2.Bits.AtomicOpRouting) {
    PciIoDevice->DeviceState.AtomicOp &= ~BIT1;
  }
  if (PciIoDevice->DeviceState.AtomicOp !=
      BitFieldRead16 (PciIoDevice->PciExpressCapability.DeviceControl2.Uint16, 6, 7)) {

      DEBUG ((
        DEBUG_INFO, "  %a [%02d|%02d|%02d]: %x -> %x.\n",
        __FUNCTION__, PciIoDevice->BusNumber, PciIoDevice->DeviceNumber, PciIoDevice->FunctionNumber,
        BitFieldRead16 (PciIoDevice->PciExpressCapability.DeviceControl2.Uint16, 6, 7),
        PciIoDevice->DeviceState.AtomicOp
        ));
      BitFieldWrite16 (
        PciIoDevice->PciExpressCapability.DeviceControl2.Uint16, 6, 7,
        PciIoDevice->DeviceState.AtomicOp
        );
      return PciIoDevice->PciIo.Pci.Write (
                                    &PciIoDevice->PciIo,
                                    EfiPciIoWidthUint16,
                                    PciIoDevice->PciExpressCapabilityOffset
                                    + OFFSET_OF (PCI_CAPABILITY_PCIEXP, DeviceControl2),
                                    1,
                                    &PciIoDevice->PciExpressCapability.DeviceControl2.Uint16
                                    );
  }

  return EFI_SUCCESS;
}

/**
  Record the parent Root Port 10b Extended Tag Completer capability.

  @param PciDevice              A pointer to the PCI_IO_DEVICE.
  @param Level                  The level of the PCI device in the heirarchy.
                                Level of root ports is 0.
  @param Context                Pointer to feature specific context.
**/
STATIC
VOID
ExtendedTagCheck (
  IN PCI_IO_DEVICE *PciDevice,
  IN UINTN         Level,
  IN VOID          **Context
  )
{
  BOOLEAN                          *TenBitCompleterCapable;

  DEBUG ((
    DEBUG_INFO, "  %a [%02d|%02d|%02d]: Capability = %x",
    __FUNCTION__, PciDevice->BusNumber, PciDevice->DeviceNumber, PciDevice->FunctionNumber,
    PciDevice->PciExpressCapability.DeviceCapability.Bits.ExtendedTagField
    ));
  DEBUG ((
    DEBUG_INFO, "  Capability2 = [%x, %x]\n",
    PciDevice->PciExpressCapability.DeviceCapability2.Bits.TenBitTagRequesterSupported,
    PciDevice->PciExpressCapability.DeviceCapability2.Bits.TenBitTagCompleterSupported
    ));

  TenBitCompleterCapable = *Context;
  if (TenBitCompleterCapable == NULL) {
    TenBitCompleterCapable = AllocatePool (sizeof (*TenBitCompleterCapable));
    *Context = TenBitCompleterCapable;
  }
  if (Level == 1) {
    *TenBitCompleterCapable = (BOOLEAN)
        (PciDevice->PciExpressCapability.DeviceCapability2.Bits.TenBitTagCompleterSupported);
  }
}

/**
  Program PCIe feature ExtendedTag.

  @param PciIoDevice            A pointer to the PCI_IO_DEVICE.
  @param Level                  The level of the PCI device in the heirarchy.
                                Level of root ports is 0.
  @param Context                Pointer to feature specific context.

  @retval EFI_SUCCESS           setup of PCI feature ExtendedTag is successful.
  @retval EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
                                valid for the PCI configuration header of the PCI controller.
  @retval EFI_INVALID_PARAMETER Buffer is NULL or Width is invalid.
**/
EFI_STATUS
ExtendedTagProgram (
  IN  PCI_IO_DEVICE *PciIoDevice,
  IN  UINTN         Level,
  IN  VOID          **Context
  )
{
  BOOLEAN                       *TenBitCompleterCapable;
  PCI_REG_PCIE_DEVICE_CONTROL2  DeviceCtl2;
  PCI_REG_PCIE_DEVICE_CONTROL   DeviceCtl;
  EFI_STATUS                    Status;

  if (PciIoDevice->DeviceState.ExtendedTag == EFI_PCI_EXPRESS_DEVICE_POLICY_AUTO ||
      PciIoDevice->DeviceState.ExtendedTag == EFI_PCI_EXPRESS_DEVICE_POLICY_NOT_APPLICABLE) {
    return EFI_SUCCESS;
  }

  //
  // BIT0 of the policy value is for 5b or 8b Extended Tag (DeviceControl BIT8)
  // BIT1 of the policy value is for 10b Extended Tag (DeviceControl2 BIT12)
  //
  if ((PciIoDevice->DeviceState.ExtendedTag >> 2) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // check and prepare the context for the Extended Tag Completer capability
  //
  ExtendedTagCheck (PciIoDevice, Level, Context);

  //
  // start with no change to device 10b Requester Enable state
  //
  DeviceCtl2.Bits.TenBitTagRequesterEnable = 0;

  //
  // the device should be capable of 10b Extended Tag Requester
  //
  if ((PciIoDevice->DeviceState.ExtendedTag & BIT1) &&
      (PciIoDevice->PciExpressCapability.DeviceCapability2.Bits.TenBitTagRequesterSupported)) {
    //
    // for the Endpoint device 10b Extended Tag Requester Enable, the RC should be
    // 10b Completer capable
    //
    if (PciIoDevice->PciExpressCapability.Capability.Bits.DevicePortType == PCIE_DEVICE_PORT_TYPE_PCIE_ENDPOINT ||
        PciIoDevice->PciExpressCapability.Capability.Bits.DevicePortType == PCIE_DEVICE_PORT_TYPE_LEGACY_PCIE_ENDPOINT) {
      //
      // check the parent Root Port 10b Extended Tag Completer Capability
      //
      TenBitCompleterCapable = *Context;
      if (*TenBitCompleterCapable == TRUE) {
        //
        // since the RC is 10b COmpleter capable, enable the EP as 10b Requester
        //
        DeviceCtl2.Bits.TenBitTagRequesterEnable = 1;
      }
    } else {
      //
      // enable the device as 10b Requester if it is capable and per platform ask
      //
      DeviceCtl2.Bits.TenBitTagRequesterEnable = 1;
    }
    //
    // write DeviceControl2 register for 10b Extended Tag Requester state
    //
    if (DeviceCtl2.Bits.TenBitTagRequesterEnable !=
        PciIoDevice->PciExpressCapability.DeviceControl2.Bits.TenBitTagRequesterEnable) {

        DEBUG ((
          DEBUG_INFO, "  %a [%02d|%02d|%02d]: %x -> %x.\n",
          __FUNCTION__, PciIoDevice->BusNumber, PciIoDevice->DeviceNumber, PciIoDevice->FunctionNumber,
          PciIoDevice->PciExpressCapability.DeviceControl2.Bits.TenBitTagRequesterEnable,
          DeviceCtl2.Bits.TenBitTagRequesterEnable
          ));
        PciIoDevice->PciExpressCapability.DeviceControl2.Bits.TenBitTagRequesterEnable =
          DeviceCtl2.Bits.TenBitTagRequesterEnable;

        Status = PciIoDevice->PciIo.Pci.Write (
                                        &PciIoDevice->PciIo,
                                        EfiPciIoWidthUint16,
                                        PciIoDevice->PciExpressCapabilityOffset
                                        + OFFSET_OF (PCI_CAPABILITY_PCIEXP, DeviceControl2),
                                        1,
                                        &PciIoDevice->PciExpressCapability.DeviceControl2.Uint16
                                        );
        if (EFI_ERROR(Status)) {
          return Status;
        }
    }
  }

  //
  // if no 10b Extended Tag Requester for this device than consider 8b or 5b Extended Requester
  //
  if (!DeviceCtl2.Bits.TenBitTagRequesterEnable) {
    //
    // the device should be capable of 8b Extended Tag Requester
    //
    DeviceCtl.Bits.ExtendedTagField = (UINT16)
              ((PciIoDevice->DeviceState.ExtendedTag & BIT0) &&
               (PciIoDevice->PciExpressCapability.DeviceCapability.Bits.ExtendedTagField));

    if (DeviceCtl.Bits.ExtendedTagField !=
        PciIoDevice->PciExpressCapability.DeviceControl.Bits.ExtendedTagField) {
      DEBUG ((
          DEBUG_INFO, "  %a [%02d|%02d|%02d]: %x -> %x.\n",
          __FUNCTION__, PciIoDevice->BusNumber, PciIoDevice->DeviceNumber, PciIoDevice->FunctionNumber,
          PciIoDevice->PciExpressCapability.DeviceControl.Bits.ExtendedTagField,
          DeviceCtl.Bits.ExtendedTagField
          ));
      PciIoDevice->PciExpressCapability.DeviceControl.Bits.ExtendedTagField = DeviceCtl.Bits.ExtendedTagField;

      return PciIoDevice->PciIo.Pci.Write (
                                    &PciIoDevice->PciIo,
                                    EfiPciIoWidthUint16,
                                    PciIoDevice->PciExpressCapabilityOffset
                                    + OFFSET_OF (PCI_CAPABILITY_PCIEXP, DeviceControl),
                                    1,
                                    &PciIoDevice->PciExpressCapability.DeviceControl.Uint16
                                    );
    }
  }

  return EFI_SUCCESS;
}

