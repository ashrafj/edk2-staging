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

