import { Component } from '@angular/core';
@Component({
    selector: 'observer-tabs',
    template: require('./observer-tabs.html'),
    styles: [require('./observer-tabs.scss')]
})
export class ObserverTabsComponent {
    private selectedTab = 'dragRace';
    private started = false;

    startRace() {
        this.started = true;
    }

    stopRace() {
        this.started = false;
    }
}