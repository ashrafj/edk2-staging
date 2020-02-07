/** @file
  PCI standard feature support functions implementation for PCI Bus module..

Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _EFI_PCI_EXPRESS_FEATURES_H_
#define _EFI_PCI_EXPRESS_FEATURES_H_


/**
  The main routine which process the PCI feature Max_Payload_Size as per the
  device-specific platform policy, as well as in complaince with the PCI Base
  specification Revision 4, that aligns the value for the entire PCI heirarchy
  starting from its physical PCI Root port / Bridge device.

  @param PciDevice                      A pointer to the PCI_IO_DEVICE.
  @param PciFeaturesConfigurationTable  pointer to PCI_EXPRESS_FEATURES_CONFIGURATION_TABLE

  @retval EFI_SUCCESS                   processing of PCI feature Max_Payload_Size
                                        is successful.
**/
EFI_STATUS
SetupMaxPayloadSize (
  IN  PCI_IO_DEVICE                             *PciDevice,
  IN  PCI_EXPRESS_FEATURES_CONFIGURATION_TABLE  *PciFeaturesConfigurationTable
  );

EFI_STATUS
CasMaxPayloadSize (
  IN  PCI_IO_DEVICE                             *PciDevice,
  IN  PCI_EXPRESS_FEATURES_CONFIGURATION_TABLE  *PciFeaturesConfigurationTable
  );

/**
  Overrides the PCI Device Control register Max_Read_Req_Size register field; if
  the hardware value is different than the intended value.

  @param  PciDevice             A pointer to the PCI_IO_DEVICE instance.

  @retval EFI_SUCCESS           The data was read from or written to the PCI controller.
  @retval EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
                                valid for the PCI configuration header of the PCI controller.
  @retval EFI_INVALID_PARAMETER Buffer is NULL or Width is invalid.

**/
EFI_STATUS
ProgramMaxPayloadSize (
  IN PCI_IO_DEVICE          *PciDevice,
  IN VOID                   *PciExFeatureConfiguration
  );


EFI_STATUS
ConditionalCasMaxReadReqSize (
  IN  PCI_IO_DEVICE                             *PciDevice,
  IN  PCI_EXPRESS_FEATURES_CONFIGURATION_TABLE  *PciFeaturesConfigurationTable
  );

/**
  The main routine which process the PCI feature Max_Read_Req_Size as per the
  device-specific platform policy, as well as in complaince with the PCI Base
  specification Revision 4, that aligns the value for the entire PCI heirarchy
  starting from its physical PCI Root port / Bridge device.

  @param PciDevice                      A pointer to the PCI_IO_DEVICE.
  @param PciConfigPhase                 for the PCI feature configuration phases:
                                        PciExpressFeatureSetupPhase & PciExpressFeatureEntendedSetupPhase
  @param PciFeaturesConfigurationTable  pointer to PCI_EXPRESS_FEATURES_CONFIGURATION_TABLE

  @retval EFI_SUCCESS                   processing of PCI feature Max_Read_Req_Size
                                        is successful.
**/
EFI_STATUS
SetupMaxReadReqSize (
  IN  PCI_IO_DEVICE                           *PciDevice,
  IN  PCI_EXPRESS_FEATURES_CONFIGURATION_TABLE  *PciFeaturesConfigurationTable
  );

/**
  Overrides the PCI Device Control register Max_Read_Req_Size register field; if
  the hardware value is different than the intended value.

  @param  PciDevice             A pointer to the PCI_IO_DEVICE instance.

  @retval EFI_SUCCESS           The data was read from or written to the PCI controller.
  @retval EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
                                valid for the PCI configuration header of the PCI controller.
  @retval EFI_INVALID_PARAMETER Buffer is NULL or Width is invalid.

**/
EFI_STATUS
ProgramMaxReadReqSize (
  IN PCI_IO_DEVICE          *PciDevice,
  IN VOID                   *PciExFeatureConfiguration
  );

/**
  Overrides the PCI Device Control register Relax Order register field; if
  the hardware value is different than the intended value.

  @param  PciDevice             A pointer to the PCI_IO_DEVICE instance.

  @retval EFI_SUCCESS           The data was read from or written to the PCI device.
  @retval EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
                                valid for the PCI configuration header of the PCI controller.
  @retval EFI_INVALID_PARAMETER Buffer is NULL or Width is invalid.

**/
EFI_STATUS
ProgramRelaxOrder (
  IN PCI_IO_DEVICE          *PciDevice,
  IN VOID                   *PciExFeatureConfiguration
  );

/**
  Overrides the PCI Device Control register No-Snoop register field; if
  the hardware value is different than the intended value.

  @param  PciDevice             A pointer to the PCI_IO_DEVICE instance.

  @retval EFI_SUCCESS           The data was read from or written to the PCI device.
  @retval EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
                                valid for the PCI configuration header of the PCI controller.
  @retval EFI_INVALID_PARAMETER Buffer is NULL or Width is invalid.

**/
EFI_STATUS
ProgramNoSnoop (
  IN PCI_IO_DEVICE          *PciDevice,
  IN VOID                   *PciExFeatureConfiguration
  );

/**
  The main routine which process the PCI feature Completion Timeout as per the
  device-specific platform policy, as well as in complaince with the PCI Base
  specification Revision 4.

  @param PciDevice                      A pointer to the PCI_IO_DEVICE.
  @param PciConfigPhase                 for the PCI feature configuration phases:
                                        PciExpressFeatureSetupPhase & PciExpressFeatureEntendedSetupPhase

  @retval EFI_SUCCESS                   processing of PCI feature CTO is successful.
**/
EFI_STATUS
SetupCompletionTimeout (
  IN PCI_IO_DEVICE          *PciDevice,
  IN VOID                   *PciExFeatureConfiguration
  );

/**
  Overrides the PCI Device Control2 register Completion Timeout range; if
  the hardware value is different than the intended value.

  @param  PciDevice             A pointer to the PCI_IO_DEVICE instance.

  @retval EFI_SUCCESS           The data was read from or written to the PCI device.
  @retval EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
                                valid for the PCI configuration header of the PCI controller.
  @retval EFI_INVALID_PARAMETER Buffer is NULL or Width is invalid.

**/
EFI_STATUS
ProgramCompletionTimeout (
  IN PCI_IO_DEVICE          *PciDevice,
  IN VOID                   *PciExFeatureConfiguration
  );

/**
  Routine to setup the AtomicOp Requester in the PCI device, verifies the routing
  support in the bridge devices, to be complaint as per the PCI Base specification.

  @param PciDevice                      A pointer to the PCI_IO_DEVICE.
  @param PciExFeatureConfiguration      pointer to common configuration table to
                                        initialize the PCI Express feature

  @retval EFI_SUCCESS                   bridge device routing capability is successful.
          EFI_INVALID_PARAMETER         input parameter is NULL
**/
EFI_STATUS
SetupAtomicOpRoutingSupport (
  IN PCI_IO_DEVICE                              *PciDevice,
  IN PCI_EXPRESS_FEATURES_CONFIGURATION_TABLE   *PciExFeatureConfiguration
  );

/**
  Overrides the PCI Device Control 2 register AtomicOp Requester enable field; if
  the hardware value is different than the intended value.

  @param  PciDevice             A pointer to the PCI_IO_DEVICE instance.

  @retval EFI_SUCCESS           The data was read from or written to the PCI device.
  @retval EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
                                valid for the PCI configuration header of the PCI controller.
  @retval EFI_INVALID_PARAMETER Buffer is NULL or Width is invalid.

**/
EFI_STATUS
ProgramAtomicOp (
  IN PCI_IO_DEVICE          *PciDevice,
  IN VOID                   *PciExFeatureConfiguration
  );

#endif
