// hkinternal/hkAgentMachineUtil.obj: from the records 0x691980 0x691a68 0x691bc8
// @flags /TP /O2 /Oc
// @address 0x691980 ?gather_batch_agent_stats@phys_agent_machine_util@@SAXABUphys_batch_agent_track@@PAVphys_statistics_collector@@@Z
// @address 0x691bc8 ?gather_contact_manager_stats@phys_agent_machine_util@@SAXABUphys_pair_agent_track@@PAVphys_statistics_collector@@@Z
// @address 0x691a68 ?gather_pair_agent_stats@phys_agent_machine_util@@SAXABUphys_pair_agent_track@@PAVphys_statistics_collector@@@Z
#include "phys/phys_statistics_collector.h"
#include "phys/phys_agent_machine_util.h"
#include "phys/phys_batch_agent_track.h"
#include "phys/phys_batch_agent_machine.h"
#include "phys/phys_pair_agent_sector.h"
#include "phys/phys_pair_agent_track.h"
#include "phys/phys_pair_agent_entry.h"
#include "phys/phys_collide.h"
#include "phys/phys_agent.h"

void phys_agent_machine_util::gather_batch_agent_stats(const phys_batch_agent_track &track, phys_statistics_collector *collector)
{
    int track_sector_index = 0;
    phys_batch_agent_entry *command;
    phys_batch_agent_entry *sector_end;
    const char *agent_label = "Agent";
    const char *sector_label = "Sector";
    for (;;) {
        phys_batch_agent_sector *sector = track.m_track_sectors[track_sector_index];
        command = (phys_batch_agent_entry *)sector->get_track_begin();
        sector_end = (phys_batch_agent_entry *)sector->get_track_end();
        collector->add_chunk_stats(sector_label, phys_statistics_collector::stat_category_runtime, sector, sector->m_bytes_allocated + 16, 512);
        track_sector_index++;
        do {
            switch (command->m_stream_command) {
            case 6: {
                phys_batch_agent_entry *entry = command;
                const phys_ref_object *pair_agent_entry = entry->m_stream_agent;
                collector->add_child_object(agent_label, phys_statistics_collector::stat_category_runtime, pair_agent_entry);
                command = phys_add_byte_offset(command, entry->m_size);
                break;
            }
            case 0:
            case 2:
            case 3:
            case 4:
            case 5: {
                phys_batch_agent_entry *entry = command;
                command = phys_add_byte_offset(command, entry->m_size);
                break;
            }
            case 1:
                return;
            default:
                break;
            }
        } while (command < sector_end);
    }
}

void phys_agent_machine_util::gather_pair_agent_stats(const phys_pair_agent_track &track, phys_statistics_collector *collector)
{
    for (int track_sector_index = 0; track_sector_index < track.m_track_sectors.getSize(); track_sector_index++) {
        phys_pair_agent_sector *sector = track.m_track_sectors[track_sector_index];
        int sector_bytes = (track_sector_index + 1 == track.m_track_sectors.getSize()) ? track.m_last_sector_bytes : 0x200;
        collector->add_chunk_stats("Sector", phys_statistics_collector::stat_category_runtime, sector, sector_bytes, 0x200);
    }
    for (int track_sector_index = 0; track_sector_index < track.m_track_sectors.getSize();) {
        phys_pair_agent_sector *sector = track.m_track_sectors[track_sector_index];
        phys_pair_agent_entry *entry = sector->get_track_begin();
        track_sector_index++;
        phys_pair_agent_entry *sector_end = (track_sector_index == track.m_track_sectors.getSize()) ? phys_add_byte_offset(entry,
            track.m_last_sector_bytes) : sector->get_track_end();
        while (entry < sector_end) {
            switch (entry->m_stream_command) {
            case 6:
                collector->add_child_object("Agent", phys_statistics_collector::stat_category_runtime, entry->m_stream_agent);
            case 2:
            case 4:
                collector->add_child_object("ContactMgr", phys_statistics_collector::stat_category_runtime, entry->m_contact_manager);
                break;
            default:
                break;
            }
            entry = phys_add_byte_offset(entry, entry->m_size);
        }
    }
}

void phys_agent_machine_util::gather_contact_manager_stats(const phys_pair_agent_track &track, phys_statistics_collector *collector)
{
    for (int track_sector_index = 0; track_sector_index < track.m_track_sectors.getSize();) {
        phys_pair_agent_entry *sector = track.m_track_sectors[track_sector_index]->get_track_begin();
        phys_pair_agent_entry *entry = sector;
        track_sector_index++;
        phys_pair_agent_entry *sector_end = (track_sector_index == track.m_track_sectors.getSize())
            ? phys_add_byte_offset(entry, track.m_last_sector_bytes) : phys_add_byte_offset(sector, 0x200);
        while (entry < sector_end) {
            switch (entry->m_stream_command) {
            case 2:
            case 4:
            case 6:
                collector->add_child_object("ContactMgr", phys_statistics_collector::stat_category_runtime, entry->m_contact_manager);
                break;
            default:
                break;
            }
            entry = phys_add_byte_offset(entry, entry->m_size);
        }
    }
}

