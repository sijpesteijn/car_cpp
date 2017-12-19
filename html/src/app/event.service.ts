import {EventEmitter, Injectable} from "@angular/core";

export const OBSERVER_ROI_SET = 'OBSERVER_ROI_SET';
export const OBSERVER_ROI_LOADED = 'OBSERVER_ROI_LOADED';
export const OBSERVER_ROIS_CLEAR = 'OBSERVER_ROIS_CLEAR';

@Injectable()
export class EventService {

    private events: EventEmitter<any> = new EventEmitter<any>();

    emit(event: string, data?: any) {
        this.events.emit({ name: event, data: data });
    }

    subscribe(callback: any) {
        return this.events.subscribe(callback);
    }

}
