Component( "BGQ-core" )
Program( "BGQ-core", "comm_test1.txt" )

Property( "BGQ-core", "app.transferSize", lambda gid, cid, cids, index: 2 )

Property( "BGQ-core", "mpi.commRank", lambda gid, cid, cids, index: cid  )
Property( "BGQ-core", "mpi.commSize", lambda gid, cid, cids, index: cids )

Ordinal( "BGQ-core", "mpi.commRank" )
Relation( "BGQ-core", "BGQ-core", "cpu", "self" )
Attribute( "BGQ-core", "usage", 0.0 )

Operation( "BGQ-core", "unwait", NoLookup, Dawdle(0))

Mailbox( "BGQ-core", "unwait", lambda source, target, size, tag: [source],
         OnTarget )

Component( "BGQ-network" )
Attribute( "BGQ-network", "usage", 0.0 )
Operation( "BGQ-network", "transfer", "comm_test1.csv",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( Outputs(0) ),
           Modify( "usage", 0.0 ) )

Mailbox( "BGQ-network", "transfer", lambda source, target, size, tag: [size],
         OnAll )

Component( "system" )
Offspring( "system", Torus("BGQ-core", "BGQ-network", [16, 16, 8]) )
Root("system")

