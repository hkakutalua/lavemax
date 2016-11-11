CREATE TABLE UserAccount(
	UserAccountId SERIAL NOT NULL,
	Name VARCHAR(100) NOT NULL,
	Login VARCHAR(100) UNIQUE NOT NULL,
	Password VARCHAR(100) NOT NULL,
	IsActive BOOLEAN DEFAULT TRUE,
	PrivilegeLevel VARCHAR(30) NOT NULL,

	CONSTRAINT PkUserAccount PRIMARY KEY(UserAccountId),
	
	/* Níveis de privilégios */
	/* 	admin: it's a god. It creates and destroys everything without mercy
		manager: half-god like Hercules, he only manages (create and eliminate)
				 users, receipts and services
		user: human, he only creates and search for invoices
	*/
	CONSTRAINT CheckPrivilegeLevel
		CHECK(PrivilegeLevel IN('admin','manager','user'))
);
CREATE INDEX UserLoginPasswordIndex ON UserAccount(Login, Password);
CREATE INDEX UserIsActiveIndex ON UserAccount(IsActive);
CREATE INDEX UserPrivilegeLevel ON UserAccount(PrivilegeLevel);

CREATE TABLE Client(
	ClientId SERIAL NOT NULL,
	Name VARCHAR(100) NOT NULL,
	PhoneNumber VARCHAR(50) NOT NULL,
	Address VARCHAR(50) NOT NULL,
	
	CONSTRAINT PkClient PRIMARY KEY(ClientId)
);
CREATE INDEX ClientNameLoweredIndex ON Client(lower(Name) varchar_pattern_ops);
CREATE INDEX ClientPhoneNumberIndex ON Client(PhoneNumber varchar_pattern_ops);

CREATE TABLE Receipt
(
	ReceiptNumber INTEGER NOT NULL,
	UserAccountId INTEGER NOT NULL,
	ClientId      INTEGER NOT NULL,
	
	/* Delivery */
	RegistrationDate TIMESTAMP NOT NULL,
	DeliveryDate 	 TIMESTAMP NOT NULL,

	CONSTRAINT PkReceipt  PRIMARY KEY(ReceiptNumber),
	CONSTRAINT FkUserAccount FOREIGN KEY(UserAccountId)
		REFERENCES UserAccount(UserAccountId),
	CONSTRAINT FkClient FOREIGN KEY(ClientId)
		REFERENCES Client(ClientId)
);
CREATE INDEX ReceiptRegistrationDateIndex ON Receipt(RegistrationDate);

CREATE TABLE ReceiptPayment
(
	ReceiptNumber INTEGER NOT NULL,

	/* There are two kinds of payment: complete and partial
	   -- Complete payments occurs when the client pays all the money charged
	   -- Partial payments occurs when the client pays part of the money,
		then the rest is payed later
	*/
	PaymentType VARCHAR(10) NOT NULL,
	
	/* Details about payment */
	Discount	SMALLINT DEFAULT 0,
	TotalToPayGross	DECIMAL(10,2) NOT NULL,
	TotalToPayNet	DECIMAL(10,2) NOT NULL,
	TotalPayed      DECIMAL(10,2) NOT NULL,

	CONSTRAINT CheckPaymentType CHECK(PaymentType IN('complete','partial')),
	CONSTRAINT CheckTotalPayed  CHECK(TotalPayed <= TotalToPayNet),
	CONSTRAINT CheckDiscount    CHECK(Discount IN(0,10,20)),
	
	CONSTRAINT PkReceiptPayment PRIMARY KEY(ReceiptNumber),
	CONSTRAINT FkReceipt FOREIGN KEY(ReceiptNumber)
		REFERENCES Receipt(ReceiptNumber)
		ON DELETE CASCADE
);
CREATE INDEX ReceiptPaymentPaymentTypeIndex ON ReceiptPayment(PaymentType);

CREATE TABLE ServiceType
(
	ServiceTypeId SERIAL NOT NULL,
	Name VARCHAR(100) UNIQUE NOT NULL,

	CONSTRAINT PkServiceType PRIMARY KEY(ServiceTypeId)
);

CREATE TABLE ClothType
(
	ClothTypeId SERIAL NOT NULL,
	ServiceTypeId INTEGER NOT NULL,
	Name VARCHAR(100) NOT NULL,

	CONSTRAINT UnClothName UNIQUE(ServiceTypeId, Name),
	CONSTRAINT PkClothType PRIMARY KEY(ClothTypeId),
	CONSTRAINT FkServiceType FOREIGN KEY(ServiceTypeId)
		REFERENCES ServiceType(ServiceTypeId)
);
CREATE INDEX ClothTypeService ON ClothType(ServiceTypeId);

CREATE TABLE TreatmentType
(
	TreatmentTypeId SERIAL NOT NULL,
	ServiceTypeId INTEGER NOT NULL,
	Name VARCHAR(100) NOT NULL,

	CONSTRAINT UnTreatmentName UNIQUE(ServiceTypeId, Name),
	CONSTRAINT PkTreatmentType PRIMARY KEY(TreatmentTypeId),
	CONSTRAINT FkServiceType FOREIGN KEY(ServiceTypeId)
		REFERENCES ServiceType(ServiceTypeId)
);
CREATE INDEX TreatmentTypeService ON TreatmentType(ServiceTypeId);

CREATE TABLE Cloth
(
	ReceiptNumber 	INTEGER NOT NULL,
	ItemNumber 	INTEGER NOT NULL,

	ServiceTypeId 	INTEGER NOT NULL,
	ClothTypeId 	INTEGER NOT NULL,
	TreatmentTypeId INTEGER NOT NULL,

	PriceCharged	DECIMAL(10,2) NOT NULL,
	Urgency		INTEGER DEFAULT 0,
	
	Color		VARCHAR(50),
	Brand 		VARCHAR(50),
	Size		VARCHAR(20),
	Observations	VARCHAR(100),

	/* For square meters charged clothes */
	ChargedPerSqrMeters BOOLEAN DEFAULT FALSE,
	SizeInSqrMeters     DECIMAL(3,1),

	CONSTRAINT CheckUrgency CHECK(Urgency IN(0,50,100)),

	CONSTRAINT PkClothId PRIMARY KEY(ReceiptNumber, ItemNumber),
	CONSTRAINT FkReceiptNumber FOREIGN KEY(ReceiptNumber)
		REFERENCES Receipt(ReceiptNumber)
		ON DELETE CASCADE,
		
	CONSTRAINT FkClothTypeId FOREIGN KEY(ClothTypeId)
		REFERENCES ClothType(ClothTypeId),
	CONSTRAINT FkServiceTypeId FOREIGN KEY(ServiceTypeId)
		REFERENCES ServiceType(ServiceTypeId),
	CONSTRAINT FkTreatmentTypeId FOREIGN KEY(TreatmentTypeId)
		REFERENCES TreatmentType(TreatmentTypeId)
);

CREATE TABLE ClothTreatment
(
	ClothTypeId 	    INTEGER NOT NULL,
	TreatmentTypeId     INTEGER NOT NULL,
	Price 		    DECIMAL(10,2) NOT NULL,
	ChargedPerSqrMeters BOOLEAN DEFAULT FALSE,

	CONSTRAINT PkClothTreatment PRIMARY KEY(ClothTypeId, TreatmentTypeId),
	CONSTRAINT FkClothType FOREIGN KEY(ClothTypeId)
		REFERENCES ClothType(ClothTypeId),
	CONSTRAINT FkTreatmentType FOREIGN KEY(TreatmentTypeId)
		REFERENCES TreatmentType(TreatmentTypeId)
);