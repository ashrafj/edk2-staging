/** @file
  This file encapsulate the usage of PCI Platform Protocol

  This file define the necessary hooks used to obtain the platform
  level data and policies which could be used in the PCI Enumeration phases

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#ifndef _EFI_PCI_PLATFORM_SUPPORT_H_
#define _EFI_PCI_PLATFORM_SUPPORT_H_

/**
  This function retrieves the PCI Platform Protocol published by platform driver

**/
VOID
LocatePciPlatformProtocol (
  );

/**
  This function indicates the presence of PCI Platform driver
  @retval     TRUE or FALSE
**/
BOOLEAN
CheckPciPlatformProtocolInstall (
  );


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
  );

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
  );

/**
  This function retrieves the PCI platform policy.

  @param  PciPolicy     pointer to the legacy EFI_PCI_PLATFORM_POLICY
  @retval Status        returns the status from the PCI Platform protocol as is

**/
EFI_STATUS
PciGetPlatformPolicy (
  OUT EFI_PCI_PLATFORM_POLICY *PciPolicy
  );

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
  );

#endif
