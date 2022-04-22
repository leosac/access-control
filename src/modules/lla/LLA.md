@page page_module_lla Module: LibLogicalAccess (LLA)

LibLogicalAccess (LLA) Module Documentation {#mod_lla_main}
==========================================================

[TOC]

@brief Details configuration regarding LibLogicalAccess integration.

Introduction {#mod_lla_intro}
=============================

The LLA module integrates ISLOG's LibLogicalAccess into Leosac. This is a
as a module that provide an authentication source, therefore it shall
conforms to [those specifications](@ref auth_specc) regarding message passing.

The module lets the user read card using a wide range of RFID device. Check for
LibLogicalAccess devices support if you want to know if your device is supported.

Configuration Options {#mod_lla_user_config}
============================================

Options    | Options  | Options               | Description                                                 | Mandatory
-----------|----------|-----------------------|-------------------------------------------------------------|-----------
sources    |          |                       | Lists of all configured readers                             | YES
--->       | source   |                       | Configuration of 1 wiegand reader                           | YES
--->       | --->     | name                  | Name of the source (for use in Authentication module)       | YES
--->       | --->     | ReaderFormatComposite | LLA's reader format composite container                     | YES
--->       | --->     | polling_time          | Time in ms to wait in LLA's waitInsertion() / waitRemoval() | NO (default to 1000)



Example {#mod_lla_example}
------------------------------

This is a example of LLA possible configuration to listen to all PC/SC readers for any card type (simple CSN / UID reading):

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<module>
    <name>LLA</name>
    <file>liblla.so</file>
    <level>103</level>
    <module_config>
        <sources>
            <source>
                <name>MY_LLA/name>
                <polling_time>10000</polling_time>
                <ReaderFormatComposite>
                  <ReaderConfiguration>
                    <ReaderProvider>PCSC</ReaderProvider>
                  </ReaderConfiguration>
                  <CardsFormat>
                    <Card>
                      <type>GenericTag</type>
                      <SelectedFormat>0</SelectedFormat>
                      <FormatConfiguration/>
                    </Card>
                  </CardsFormat>
                </ReaderFormatComposite>
            </source>
        </sources>
    </module_config>
</module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

LibLogicalAccess module is really powerful in regards to access card credentials and key management.
It supports advanced security mechanisms recommended on highly sensitive facilities.

For instance, to read a Mifare DESFire EV1 credentials encoded in Wiegand 26 and secured with an AES diversified key, the configuration will looks like:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<module>
    <name>LLA</name>
    <file>liblla.so</file>
    <level>103</level>
    <module_config>
        <sources>
            <source>
                <name>MY_DESFIRE/name>
                <polling_time>10000</polling_time>
                <ReaderFormatComposite>
                  <ReaderConfiguration>
                    <ReaderProvider>PCSC</ReaderProvider>
                  </ReaderConfiguration>
                  <CardsFormat>
                  <Card>
                    <type>DESFireEV1</type>
                    <SelectedFormat>2</SelectedFormat>
                    <WriteInfo/>
                    <FormatConfiguration>
                      <Wiegand26Format type="2">
                        <FacilityCode>0</FacilityCode>
                        <Uid>0</Uid>
                      </Wiegand26Format>
                      <DESFireEV1Location>
                        <DESFireLocation>
                          <AID>1313</AID>
                          <File>0</File>
                          <Byte>0</Byte>
                          <SecurityLevel>3</SecurityLevel>
                        </DESFireLocation>
                        <UseEV1>true</UseEV1>
                        <CryptoMethod>128</CryptoMethod>
                        <UseISO7816>false</UseISO7816>
                        <ApplicationFID>0</ApplicationFID>
                        <FileFID>0</FileFID>
                      </DESFireEV1Location>
                      <DESFireAccessInfo>
                        <ReadKey>
                          <DESFireKey keyStorageType="0">
                            <KeyDiversification keyDiversificationType="NXPAV2">
                              <NXPAV2KeyDiversification>
                                <divInput/>
                                <systemIdentifier/>
                                <revertAID>false</revertAID>
                                <forceK2Use>false</forceK2Use>
                              </NXPAV2KeyDiversification>
                            </KeyDiversification>
                            <IsCiphered>false</IsCiphered>
                            <Data>11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11</Data>
                            <ComputerMemoryKeyStorage>
                              <KeyStorage>
                                <metadata>
                                <IsHidden>False</IsHidden>
                                <IsKeyCeremony>False</IsKeyCeremony>
                                </metadata>
                              </KeyStorage>
                            </ComputerMemoryKeyStorage>
                            <KeyType>128</KeyType>
                            <KeyVersion>0</KeyVersion>
                          </DESFireKey>
                        </ReadKey>
                        <ReadKeyno>1</ReadKeyno>
                      </DESFireAccessInfo>
                    </FormatConfiguration>
                  </Card>
                </CardsFormat>
              </ReaderFormatComposite>
            </source>
        </sources>
    </module_config>
</module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To read another DESFire EV1 credential using a custom 32-bit format and to store securely the authentication key on a EAL5+ certified key storage (eg. NXP SAM AV2 chip) on the Leosac controller, you would use a configuration similar to:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<module>
    <name>LLA</name>
    <file>liblla.so</file>
    <level>103</level>
    <module_config>
        <sources>
            <source>
                <name>MY_DESFIRE_WITH_SAM/name>
                <polling_time>10000</polling_time>
                <ReaderFormatComposite>
                	<ReaderConfiguration>
                		<ReaderProvider>PCSC</ReaderProvider>
                		<ReaderUnit type="PCSC">
                			<PCSCReaderUnitConfiguration>
                				<SAMType>SAM_AV2</SAMType>
                				<SAMReaderName>OMNIKEY CardMan 5x21 0</SAMReaderName>
                				<SAMKey>
                				<KeyNo>1</KeyNo>
                				<DESFireKey keyStorageType="0">
                					<IsCiphered>false</IsCiphered>
                					<Data>AA AA AA AA AA AA AA AA AA AA AA AA AA AA AA AA</Data>
                					<ComputerMemoryKeyStorage>
                						<KeyStorage>
                							<metadata>
                							<IsHidden>False</IsHidden>
                							<IsKeyCeremony>False</IsKeyCeremony>
                							</metadata>
                						</KeyStorage>
                					</ComputerMemoryKeyStorage>
                					<KeyType>128</KeyType>
                					<KeyVersion>0</KeyVersion>
                				</DESFireKey>
                				</SAMKey>
                				<CheckSAMReaderIsAvailable>true</CheckSAMReaderIsAvailable>
                				<AutoConnectToSAMReader>true</AutoConnectToSAMReader>
                				<TransmissionProtocol>3</TransmissionProtocol>
                				<ShareMode>2</ShareMode>
                			</PCSCReaderUnitConfiguration>
                			<TransportType/>
                			<Name>OMNIKEY CardMan 5x21-CL 0</Name>
                		</ReaderUnit>
                	</ReaderConfiguration>
                	<CardsFormat>
                		<Card>
                			<type>DESFireEV1</type>
                			<SelectedFormat>16</SelectedFormat>
                			<WriteInfo/>
                			<FormatConfiguration>
                				<CustomFormat type="16">
                					<Name>Custom</Name>
                					<Fields>
                						<NumberDataField>
                							<Name>Uid</Name>
                							<Position>0</Position>
                							<IsFixedField>false</IsFixedField>
                							<IsIdentifier>true</IsIdentifier>
                							<DataRepresentation>4</DataRepresentation>
                							<DataType>3</DataType>
                							<Length>32</Length>
                							<Value>0</Value>
                						</NumberDataField>
                					</Fields>
                				</CustomFormat>
                				<DESFireEV1Location>
                					<DESFireLocation>
                						<AID>1313</AID>
                						<File>0</File>
                						<Byte>0</Byte>
                						<SecurityLevel>3</SecurityLevel>
                					</DESFireLocation>
                					<UseEV1>true</UseEV1>
                					<CryptoMethod>128</CryptoMethod>
                					<UseISO7816>false</UseISO7816>
                					<ApplicationFID>0</ApplicationFID>
                					<FileFID>0</FileFID>
                				</DESFireEV1Location>
                				<DESFireAccessInfo>
                					<ReadKey>
                						<DESFireKey keyStorageType="2">
                						<IsCiphered>false</IsCiphered>
                						<Data/>
                						<SAMKeyStorage>
                							<KeySlot>2</KeySlot>
                							<DumpKey>false</DumpKey>
                						</SAMKeyStorage>
                						<KeyType>128</KeyType>
                						<KeyVersion>1</KeyVersion>
                						</DESFireKey>
                					</ReadKey>
                					<ReadKeyno>1</ReadKeyno>
                				</DESFireAccessInfo>
                			</FormatConfiguration>
                		</Card>
                	</CardsFormat>
                </ReaderFormatComposite>
            </source>
        </sources>
    </module_config>
</module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

PC/SC technology is unlikely to be used for access control RFID readers, for that you may prefer RS485 based readers, like those
supporting OSDP protocol. OSDP exists on v1 (unrecommended) and v2 version to transmit securely the card identifier on a secure channel.
HID readers support an extension of the OSDP standard for "transparent communication". Here is an example how to use OSDP transparent communication with LLA module:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.xml
<module>
    <name>LLA</name>
    <file>liblla.so</file>
    <level>103</level>
    <module_config>
        <sources>
            <source>
                <name>MY_DESFIRE_WITH_SAM/name>
                <polling_time>10000</polling_time>
                <ReaderFormatComposite>
                	<ReaderConfiguration>
                		<ReaderProvider>OSDP</ReaderProvider>
                		<ReaderUnit type="OSDP">
                			<OSDPReaderUnitConfiguration>
                				<RS485Address>0</RS485Address>
                				<AES128Key keyStorageType="0">
                					<IsCiphered>false</IsCiphered>
                					<Data>30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f</Data>
                					<ComputerMemoryKeyStorage>
                						<KeyStorage>
                							<metadata>
                								<IsHidden>False</IsHidden>
                								<IsKeyCeremony>False</IsKeyCeremony>
                							</metadata>
                						</KeyStorage>
                					</ComputerMemoryKeyStorage>
                				</AES128Key>
                				<AES128Key keyStorageType="0">
                					<IsCiphered>false</IsCiphered>
                					<Data>30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f</Data>
                					<ComputerMemoryKeyStorage>
                						<KeyStorage>
                							<metadata>
                								<IsHidden>False</IsHidden>
                								<IsKeyCeremony>False</IsKeyCeremony>
                							</metadata>
                						</KeyStorage>
                					</ComputerMemoryKeyStorage>
                				</AES128Key>
                				<AES128Key keyStorageType="0">
                					<IsCiphered>false</IsCiphered>
                					<Data>30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f</Data>
                					<ComputerMemoryKeyStorage>
                						<KeyStorage>
                							<metadata>
                								<IsHidden>False</IsHidden>
                								<IsKeyCeremony>False</IsKeyCeremony>
                							</metadata>
                						</KeyStorage>
                					</ComputerMemoryKeyStorage>
                				</AES128Key>
                			</OSDPReaderUnitConfiguration>
                			<TransportType>SerialPort</TransportType>
                			<SerialPortDataTransport type="SerialPort">
                				<PortBaudRate>115200</PortBaudRate>
                				<SerialPort>
                					<Device>/dev/tty1</Device>
                				</SerialPort>
                			</SerialPortDataTransport>
                		</ReaderUnit>
                	</ReaderConfiguration>
                	<CardsFormat>
                		<Card>
                			<type>DESFireEV1</type>
                			<SelectedFormat>16</SelectedFormat>
                			<WriteInfo/>
                			<FormatConfiguration>
                				<CustomFormat type="16">
                					<Name>Custom</Name>
                					<Fields>
                						<NumberDataField>
                							<Name>Uid</Name>
                							<Position>0</Position>
                							<IsFixedField>false</IsFixedField>
                							<IsIdentifier>true</IsIdentifier>
                							<DataRepresentation>4</DataRepresentation>
                							<DataType>3</DataType>
                							<Length>32</Length>
                							<Value>0</Value>
                						</NumberDataField>
                					</Fields>
                				</CustomFormat>
                				<DESFireEV1Location>
                					<DESFireLocation>
                						<AID>1313</AID>
                						<File>0</File>
                						<Byte>0</Byte>
                						<SecurityLevel>3</SecurityLevel>
                					</DESFireLocation>
                					<UseEV1>true</UseEV1>
                					<CryptoMethod>128</CryptoMethod>
                					<UseISO7816>false</UseISO7816>
                					<ApplicationFID>0</ApplicationFID>
                					<FileFID>0</FileFID>
                				</DESFireEV1Location>
                				<DESFireAccessInfo>
                					<ReadKey>
                						<DESFireKey keyStorageType="0">
                							<IsCiphered>false</IsCiphered>
                							<Data/>
                							<ComputerMemoryKeyStorage>
                								<KeyStorage>
                									<metadata>
                										<IsHidden>False</IsHidden>
                										<IsKeyCeremony>False</IsKeyCeremony>
                									</metadata>
                								</KeyStorage>
                							</ComputerMemoryKeyStorage>
                							<KeyType>128</KeyType>
                							<KeyVersion>1</KeyVersion>
                						</DESFireKey>
                					</ReadKey>
                					<ReadKeyno>1</ReadKeyno>
                				</DESFireAccessInfo>
                			</FormatConfiguration>
                		</Card>
                	</CardsFormat>
                </ReaderFormatComposite>
            </source>
        </sources>
    </module_config>
</module>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Notes {#mod_lla_notes}
==========================

Technical implementations details can be found [here](@ref Leosac::Module::LLA).
