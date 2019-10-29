/** @file
  PCI standard feature support functions implementation for PCI Bus module..

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PciBus.h"
#include "PciFeatureSupport.h"

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
