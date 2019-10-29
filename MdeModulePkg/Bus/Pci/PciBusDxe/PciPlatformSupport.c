/** @file
  This file encapsulate the usage of PCI Platform Protocol

  This file define the necessary hooks used to obtain the platform
  level data and policies which could be used in the PCI Enumeration phases

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PciBus.h"

EFI_PCI_PLATFORM_PROTOCOL                     *mPciPlatformProtocol;
EFI_PCI_OVERRIDE_PROTOCOL                     *mPciOverrideProtocol;



/**
  This function retrieves the PCI Platform Protocol published by platform driver

**/
VOID
LocatePciPlatformProtocol (
  )
{
    mPciPlatformProtocol = NULL;
    gBS->LocateProtocol (
        &gEfiPciPlatformProtocolGuid,
        NULL,
        (VOID **) &mPciPlatformProtocol
    );

    //
    // If PCI Platform protocol doesn't exist, try to  get Pci Override Protocol.
    //
    if (mPciPlatformProtocol == NULL) {
      mPciOverrideProtocol = NULL;
      gBS->LocateProtocol (
          &gEfiPciOverrideProtocolGuid,
          NULL,
          (VOID **) &mPciOverrideProtocol
      );
    }
}

/**
  This function indicates the presence of PCI Platform driver
  @retval     TRUE or FALSE
**/
BOOLEAN
CheckPciPlatformProtocolInstall (
  )
{

    if (mPciPlatformProtocol != NULL) {
      return TRUE;
    } else if (mPciOverrideProtocol != NULL){
      return TRUE;
    }

  return FALSE;
}

/**
  Provides the hooks from the PCI bus driver to every PCI controller (device/function) at various
  stages of the PCI enumeration process that allow the host bridge driver to preinitialize individual
  PCI controllers before enumeration.

  This function is called during the PCI enumeration process. No specific action is expected from this
  member function. It allows the host bridge driver to preinitialize individual PCI controllers before
  enumeration.

  @param[in] HostBridgeHandle     The associated PCI host bridge handle.
  @param[in] RootBridgeHandle     The associated PCI root bridge handle.
  @param[in] RootBridgePciAddress The address of the PCI device on the PCI bus.
  @param[in] Phase          The phase of the PCI controller enumeration.
  @param[in] ExecPhase      Defines the execution phase of the PCI chipset driver.

  @retval    Status         returns the status from the PCI Platform protocol as is

**/
EFI_STATUS
PciPlatformPreprocessController (
  IN EFI_HANDLE                                    HostBridgeHandle,
  IN EFI_HANDLE                                    RootBridgeHandle,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS   RootBridgePciAddress,
  IN EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE  Phase,
  IN EFI_PCI_EXECUTION_PHASE                       ExecPhase
  )
{
  EFI_STATUS  Status;
    if (mPciPlatformProtocol != NULL) {
      //
      // Call PlatformPci::PrepController() if the protocol is present.
      //
      Status = mPciPlatformProtocol->PlatformPrepController (
                                      mPciPlatformProtocol,
                                      HostBridgeHandle,
                                      RootBridgeHandle,
                                      RootBridgePciAddress,
                                      Phase,
                                      ExecPhase
                                    );
    } else if (mPciOverrideProtocol != NULL) {
      //
      // Call PlatformPci::PrepController() if the protocol is present.
      //
      Status = mPciOverrideProtocol->PlatformPrepController (
                                      mPciOverrideProtocol,
                                      HostBridgeHandle,
                                      RootBridgeHandle,
                                      RootBridgePciAddress,
                                      Phase,
                                      ExecPhase
                                    );
    } else {
      //
      // return PCI Platform Protocol not found
      //
      return EFI_NOT_FOUND;
    }
  return Status;
}

/**
  This function notifies the PCI Platform driver about the PCI host bridge resource
  allocation phase and PCI execution phase.

  @param[in]  HostBridge     The handle of the host bridge controller.
  @param[in]  Phase          The phase of the PCI bus enumeration.
  @param[in]  ExecPhase      Defines the execution phase of the PCI chipset driver.
  @retval     Status          returns the status from the PCI Platform protocol as is

**/
EFI_STATUS
PciPlatformNotifyPhase (
  IN  EFI_HANDLE                                      HostBridgeHandle,
  IN  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE   Phase,
  IN  EFI_PCI_EXECUTION_PHASE                         ExecPhase
  )
{
  EFI_STATUS  Status;


    if (mPciPlatformProtocol != NULL) {
      Status = mPciPlatformProtocol->PlatformNotify (
                                      mPciPlatformProtocol,
                                      HostBridgeHandle,
                                      Phase,
                                      ExecPhase
                                    );
    } else if (mPciOverrideProtocol != NULL){
      Status = mPciOverrideProtocol->PlatformNotify (
                                      mPciOverrideProtocol,
                                      HostBridgeHandle,
                                      Phase,
                                      ExecPhase
                                    );
    } else {
      //
      // return PCI Platform Protocol not found
      //
      return EFI_NOT_FOUND;
    }
  return Status;
}

/**
  This function retrieves the PCI platform policy.

  @param  PciPolicy     pointer to the legacy EFI_PCI_PLATFORM_POLICY
  @retval Status        returns the status from the PCI Platform protocol as is

**/
EFI_STATUS
PciGetPlatformPolicy (
  OUT EFI_PCI_PLATFORM_POLICY *PciPolicy
  )
{
  EFI_STATUS  Status;
    if (mPciPlatformProtocol != NULL) {
      Status = mPciPlatformProtocol->GetPlatformPolicy (
                                      mPciPlatformProtocol,
                                      PciPolicy
                                    );
    }

    if (mPciOverrideProtocol != NULL) {
      Status = mPciOverrideProtocol->GetPlatformPolicy (
                                      mPciOverrideProtocol,
                                      PciPolicy
                                    );
    } else {
      //
      // return PCI Platform Protocol not found
      //
      return EFI_NOT_FOUND;
    }
  return Status;
}

/**
  This function retrieves the Option ROM image and size from the Platform.

  It uses the PCI_IO_DEVICE internal fields are used to store OpROM image/size

  @param Controller     An EFI handle for the PCI bus controller.
  @param PciIoDevice    A PCI_IO_DEVICE pointer to the PCI IO device to be registered.

  @retval EFI_SUCCESS            The option ROM was available for this device and loaded into memory.
  @retval EFI_NOT_FOUND          No option ROM was available for this device.
  @retval EFI_OUT_OF_RESOURCES   No memory was available to load the option ROM.
  @retval EFI_DEVICE_ERROR       An error occurred in obtaining the option ROM.

**/
EFI_STATUS
GetPlatformPciOptionRom (
  IN  EFI_HANDLE                    Controller,
  IN  PCI_IO_DEVICE                 *PciIoDevice
  )
{
  EFI_STATUS  Status;
  VOID        *PlatformOpRomBuffer;
  UINTN       PlatformOpRomSize;
    if (mPciPlatformProtocol != NULL) {
      Status = mPciPlatformProtocol->GetPciRom (
                                      mPciPlatformProtocol,
                                      PciIoDevice->Handle,
                                      &PlatformOpRomBuffer,
                                      &PlatformOpRomSize
                                      );
    } else if (mPciOverrideProtocol != NULL) {
      Status = mPciOverrideProtocol->GetPciRom (
                                        mPciOverrideProtocol,
                                        PciIoDevice->Handle,
                                        &PlatformOpRomBuffer,
                                        &PlatformOpRomSize
                                        );
    } else {
      //
      // return PCI Platform Protocol not found
      //
      return EFI_NOT_FOUND;
    }

  if (!EFI_ERROR (Status)) {
    PciIoDevice->EmbeddedRom    = FALSE;
    PciIoDevice->RomSize        = (UINT32)PlatformOpRomSize;
    PciIoDevice->PciIo.RomSize  = PlatformOpRomSize;
    PciIoDevice->PciIo.RomImage = PlatformOpRomBuffer;
  }
  return Status;
}
