
DROP TABLE IF EXISTS `nodesOnline`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `nodesOnline` (
  `nodeID` varchar(200) DEFAULT NULL,
  UNIQUE KEY `tb_un` (`nodeID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `temperatures`;
DROP TABLE IF EXISTS `data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `data` (
  `Prefix` varchar(255) DEFAULT NULL,
  `Date` varchar(255) DEFAULT NULL,
  `Type` varchar(255) DEFAULT NULL,
  `Value` varchar(255) DEFAULT NULL,
  `Status` int(10) DEFAULT NULL,
  `nodeIP` varchar(200) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
