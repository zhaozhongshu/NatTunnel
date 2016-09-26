#include "Other.h"
#include "crc32.h"

static const int _typeId_NatType = qRegisterMetaType<NatType>("NatType");
static const int _typeId_UpnpStatus = qRegisterMetaType<UpnpStatus>("UpnpStatus");
static const int _typeId_QHostAddress = qRegisterMetaType<QHostAddress>("QHostAddress");

char * strcopy(char * dst, size_t SizeInBytes, const char * src)
{
	char * cp = dst;
	while (*cp++ = *src++)
		if (cp - dst == SizeInBytes - 1)
		{
			*cp = '\0';
			break;
		}

	return(dst);
}

QByteArray checksumThenUnpackPackage(QByteArray package)
{
	if (package.size() < 4)
		return QByteArray();
	const QByteArray content = QByteArray::fromRawData(package.constData() + 4, package.size() - 4);
	const uint32_t receivedCrc = *(uint32_t*)package.constData();
	const uint32_t actualCrc = crc32(content.constData(), content.size());
	if (receivedCrc != actualCrc)
		return QByteArray();
	return content;
}

QByteArray checksumThenUnpackPackage(QByteArray package, QByteArray userName)
{
	if (package.size() < 4)
		return QByteArray();
	const QByteArray content = QByteArray::fromRawData(package.constData() + 4, package.size() - 4);
	const uint32_t receivedCrc = *(uint32_t*)package.constData();
	uint32_t actualCrc = crc32(content.constData(), content.size());
	actualCrc = crc32(actualCrc, userName.constData(), userName.size());
	if (receivedCrc != actualCrc)
		return QByteArray();
	return content;
}

QByteArray addChecksumInfo(QByteArray package)
{
	uint32_t crc = crc32(package.constData(), package.size());
	package.insert(0, (const char*)&crc, 4);
	return package;
}

QByteArray addChecksumInfo(QByteArray package, QByteArray userName)
{
	uint32_t crc = crc32(package.constData(), package.size());
	crc = crc32(crc, userName.constData(), userName.size());
	package.insert(0, (const char*)&crc, 4);
	return package;
}

bool isSameHostAddress(const QHostAddress & a, const QHostAddress & b)
{
	if (a.protocol() == b.protocol())
		return a == b;

	if (a.protocol() == QAbstractSocket::IPv4Protocol && b.protocol() == QAbstractSocket::IPv6Protocol)
	{
		bool convertOk = false;
		return QHostAddress(b.toIPv4Address(&convertOk)) == a && convertOk;
	}
	else if (b.protocol() == QAbstractSocket::IPv4Protocol && a.protocol() == QAbstractSocket::IPv6Protocol)
	{
		bool convertOk = false;
		return QHostAddress(a.toIPv4Address(&convertOk)) == b && convertOk;
	}
	else
	{
		return a == b;
	}
}

QHostAddress tryConvertToIpv4(const QHostAddress & hostAddress)
{
	if (hostAddress.protocol() == QAbstractSocket::IPv4Protocol)
		return hostAddress;
	if (hostAddress.protocol() == QAbstractSocket::IPv6Protocol)
	{
		bool ok = false;
		auto ipv4Address = hostAddress.toIPv4Address(&ok);
		if (ok)
			return QHostAddress(ipv4Address);
	}
	return hostAddress;
}

QString getNatDescription(NatType natType)
{
	switch (natType)
	{
	case UnknownNatType:
		return "UnknownNatType";
	case PublicNetwork:
		return "PublicNetwork";
	case FullOrRestrictedConeNat:
		return "FullOrRestrictedConeNat";
	case PortRestrictedConeNat:
		return "PortRestrictedConeNat";
	case SymmetricNat:
		return "SymmetricNat";
	default:
		return "Error";
	}
}

bool isNatAddress(const QHostAddress & hostAddress)
{
	bool isIpv4 = false;
	const quint32 ipv4 = hostAddress.toIPv4Address(&isIpv4);
	if (!isIpv4)
		return false;

	const quint8 * a = (const quint8 *)&ipv4;
	if (a[3] == 10)
		return true;
	if (a[3] == 100 && a[2] >= 64 && a[2] <= 127)
		return true;
	if (a[3] == 172 && a[2] >= 16 && a[2] <= 31)
		return true;
	if (a[3] == 192 && a[2] == 168)
		return true;
	return false;
}


