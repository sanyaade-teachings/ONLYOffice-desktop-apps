/*
 * (c) Copyright Ascensio System SIA 2010-2019
 *
 * This program is a free software product. You can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License (AGPL)
 * version 3 as published by the Free Software Foundation. In accordance with
 * Section 7(a) of the GNU AGPL its Section 15 shall be amended to the effect
 * that Ascensio System SIA expressly excludes the warranty of non-infringement
 * of any third-party rights.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  PURPOSE. For
 * details, see the GNU AGPL at: http://www.gnu.org/licenses/agpl-3.0.html
 *
 * You can contact Ascensio System SIA at 20A-12 Ernesta Birznieka-Upisha
 * street, Riga, Latvia, EU, LV-1050.
 *
 * The  interactive user interfaces in modified source and object code versions
 * of the Program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU AGPL version 3.
 *
 * Pursuant to Section 7(b) of the License you must retain the original Product
 * logo when distributing the program. Pursuant to Section 7(e) we decline to
 * grant you any rights under trademark law for use of our trademarks.
 *
 * All the Product's GUI elements, including illustrations and icon sets, as
 * well as technical writing content are licensed under the terms of the
 * Creative Commons Attribution-ShareAlike 4.0 International. See the License
 * terms at http://creativecommons.org/licenses/by-sa/4.0/legalcode
 *
*/

'use strict';
$(document).ready(function() {
    const _toolmenu_tpl = `
            <div class="main-column col-left tool-menu">
              <li class="menu-item">
                <a action="recents">
                    <svg class="icon" data-iconname="home" data-precls="tool-icon">
                        <use href="#home"></use>
                    </svg>
                    <span class="text" l10n>Main</span>
                </a>
              </li>
              <!-- <li class="menu-item"><a l10n action="templates">Templates</a></li> -->
              <li class="menu-item">
                <a action="folders">
                    <svg class="icon" data-iconname="folder" data-precls="tool-icon">
                        <use href="#folder"></use>
                    </svg>
                    <span class="text" l10n>Open</span>
                </a>
              </li>
              <li class="menu-item separator"></li>
              <li class="menu-item">
                <a action="recents" data-filter="word">
                    <svg class="icon icon-colored" data-iconname="word" data-precls="tool-icon">
                        <use href="#word"></use>
                    </svg>
                    <span class="text" l10n>Documents</span>
                </a>
              </li>
              <li class="menu-item">
                <a action="recents" data-filter="cell">
                    <svg class="icon icon-colored" data-iconname="cell" data-precls="tool-icon">
                        <use href="#cell"></use>
                    </svg>
                    <span class="text" l10n>Spreadsheets</span>
                </a>
              </li>
              <li class="menu-item">
                <a action="recents" data-filter="slide">
                    <svg class="icon icon-colored" data-iconname="slide" data-precls="tool-icon">
                        <use href="#slide"></use>
                    </svg>
                    <span class="text" l10n>Presentations</span>
                </a>
              </li>
              <li class="menu-item">
                <a l10n action="recents" data-filter="pdfe">
                    <svg class="icon icon-colored" data-iconname="pdfe" data-precls="tool-icon">
                        <use href="#pdfe"></use>
                    </svg>
                    <span class="text" l10n>PDF</span>
                </a>
              </li>
              <div class="tool-quick-menu" style="display: none;">
                <h3 class="createnew" l10n>Create new</h3>
                <li class="menu-item">
                  <a action="new:docx">
                    <svg class="icon" data-iconname="docx" data-precls="img-el">
                        <use href="#docx-big"></use>
                    </svg>
                    <span class="text" l10n>DOCUMENT</span>
                  </a>
                </li>
                <li class="menu-item">
                  <a action="new:xlsx">
                    <svg class="icon" data-iconname="xlsx" data-precls="img-el">
                        <use href="#xlsx-big"></use>
                    </svg>
                    <span class="text" l10n>SPREADSHEET</span>
                  </a>
                </li>
                <li class="menu-item">
                  <a action="new:pptx">
                    <svg class="icon" data-iconname="pptx" data-precls="img-el">
                        <use href="#pptx-big"></use>
                    </svg>
                    <span class="text" l10n>PRESENTATION</span>
                  </a>
                </li>
                <li class="menu-item" style="padding-bottom:0;">
                  <a action="new:form">
                    <svg class="icon" data-iconname="form" data-precls="img-el">
                        <use href="#pdf-big"></use>
                    </svg>
                    <span class="text" l10n="newForm">MASTER FORM</span>
                  </a>
                </li>
              </div>
              <li class="menu-item devider"></li>
              <li class="menu-item">
                <a action="settings">
                    <svg class="icon" data-iconname="settings" data-precls="tool-icon">
                        <use href="#settings"></use>
                    </svg>
                    <span class="text" l10n>Settings</span>
                </a>
              </li>
              <li class="menu-item">
                <a action="about">
                    <svg class="icon" data-iconname="about" data-precls="tool-icon">
                        <use href="#about"></use>
                    </svg>
                    <span class="text" l10n>About</span>
                </a>
              </li>
            </div>
            <div class="main-column col-center after-left">
            </div>`;
    $('#placeholder').html(_toolmenu_tpl);

    $('.tool-menu').on('click', '> .menu-item > a', onActionClick);

    if ( window.utils.isWinXp ) {
        $('a[action] use').each((i, e) => {
            const _attr_href = e.getAttribute('href');
            if ( !!_attr_href ) {
                const $el = $(e), $parent = $el.parent();
                $el.remove();
                $parent.html(`<use xlink:href="${_attr_href}"></use>`);
            }
        });
    }

    !window.app && (window.app = {controller:{}});
    !window.app.controller && (window.app.controller = {});

    if ( !window.utils.isWinXp && !!window.ControllerTemplates )
        window.app.controller.templates = (new ControllerTemplates).init();
    window.app.controller.recent = (new ControllerRecent).init();
    window.app.controller.folders = (new ControllerFolders).init();
    window.app.controller.about = (new ControllerAbout).init();
    window.app.controller.settings = (new ControllerSettings).init();
    if (!!window.ControllerPortals)
        window.app.controller.portals = (new ControllerPortals({})).init();
    !!window.ControllerExternalPanel && (window.app.controller.externalpanel = (new ControllerExternalPanel({})).init());

    if (!localStorage.welcome) {
        app.controller.welcome = (new ControllerWelcome).init();
        selectAction('welcome');

        localStorage.setItem('welcome', 'have been');
    } else {
        if ( !!utils.inParams.panel && $(`.action-panel.${utils.inParams.panel}`).length )
            selectAction(utils.inParams.panel);
        else selectAction('recents');
    }

    $('#placeholder').on('click', '.newportal', function(){
        CommonEvents.fire("portal:create");
    });

    if (window.app.controller.portals && !window.config.portals.checklist) {
        $('.tools-connect').hide();
        hideAction('connect');
        console.log('There are no cloud providers');
    }

    if (!utils.inParams.waitingloader)
        setLoaderVisible(false);

    /* test information */
    // var arr = [
    //     {"id":0,"type":utils.defines.FileFormat.FILE_CROSSPLATFORM_PDF,"path":"https://testinfo.teamlab.info/New Document.docx","modifyed":"11.12.2015 18:45"},
    //     {"id":1,"type":utils.defines.FileFormat.FILE_CROSSPLATFORM_DJVU,"path":"C:\\Users\\maxim.kadushkin\\Documents\\DPIConfig_SmallPCs.docx","modifyed":"11.12.2015 18:22"},
    //     {"id":2,"type":utils.defines.FileFormat.FILE_CROSSPLATFORM_XPS,"path":"/Users/ayuzhin/Develop/Web/test.html","modifyed":"11.12.2015 17:58"},
    //     {"id":3,"type":utils.defines.FileFormat.FILE_PRESENTATION_PPTX,"path":"https://testinfo.teamlab.info\\Sadfasd.docx","modifyed":"10.12.2015 17:25"},
    //     {"id":4,"type":utils.defines.FileFormat.FILE_SPREADSHEET_CSV,"path":"https://testinfo.teamlab.info\\Sadfasd.docx","modifyed":"10.12.2015 17:25"},
    //     {"id":5,"type":utils.defines.FileFormat.FILE_SPREADSHEET_ODS,"path":"https://testinfo.teamlab.info\\Sadfasd.docx","modifyed":"10.12.2015 17:25"},
    //     {"id":6,"type":utils.defines.FileFormat.FILE_SPREADSHEET_XLS,"path":"https://testinfo.teamlab.info\\Office 365 Value Added Reseller Guide.docx","modifyed":"10.12.2015 16:46"}
    // ];
    // window.onupdaterecents(arr);
    // window.onupdaterecovers(arr);
    /* **************** */

    setTimeout(()=>{
        if (window.sdk) {
            window.sdk.LocalFileRecovers();
            window.sdk.LocalFileRecents();

            window.sdk.execCommand('app:onready', '');
        } 
    }, 50);

});

function onActionClick(e) {
    var $el = $(this);
    var action = $el.attr('action');

    if (/^custom/.test(action)) return;

    if (action == 'open' && 
            !app.controller.recent.getRecents().size() && 
                !app.controller.recent.getRecovers().size()) 
    {
        openFile(OPEN_FILE_FOLDER, '');
    } else {
        if (action === 'about') {
            return CommonEvents.fire('panel:show', [action]);
        }

        $('.tool-menu > .menu-item').removeClass('selected');
        $el.parent().addClass('selected');
        $('.action-panel').hide();
        $('.action-panel.' + action).show(0,()=>{
            // bug: recent panel has the wrong height if 'wellcome' panel is showed firstly
            if (action == 'recents') {
                app.controller.recent.filterRecents($el.data('filter'));
                // app.controller.recent.view.updatelistsize();
            }
        });

        CommonEvents.fire('panel:show', [action]);
    }
};

function selectAction(action) {
    if ( !$(`.action-panel.${action}`).length ) return;

    if (action === 'about') {
        return CommonEvents.fire('panel:show', [action]);
    }

    $('.tool-menu > .menu-item').removeClass('selected');
    $('.tool-menu a[action='+action+']').parent().addClass('selected');
    $('.action-panel').hide();
    $('.action-panel.' + action).show();

    if (action === 'recents') {
        $('.tool-menu a[data-filter]').parent().removeClass('selected');
        app.controller.recent.filterRecents(null);
    }

    CommonEvents.fire('panel:show', [action]);
};

function hideAction(action, hide) {
    var mitem = $('.tool-menu a[action='+action+']').parent();
    mitem.removeClass('extra')[hide===false?'show':'hide']();
    $('.action-panel.' + action)[hide===false?'show':'hide']();
};

function setLoaderVisible(isvisible, timeout) {
    setTimeout(function(){
        $('#loading-mask')[isvisible?'show':'hide']();
    }, timeout || 200);
}

var OPEN_FILE_RECOVERY = 1;
var OPEN_FILE_RECENT = 2;
var OPEN_FILE_FOLDER = 3;
var Scroll_offset = '16px';

{
    const mq = "screen and (-webkit-min-device-pixel-ratio: 1.01) and (-webkit-max-device-pixel-ratio: 1.99), " +
                                "screen and (min-resolution: 1.01dppx) and (max-resolution: 1.99dppx)";

    const mql = window.matchMedia(mq);
    mql.addEventListener('change', e => {
        CommonEvents.fire("icons:svg", [!e.target.matches]);
        replaceIcons(!e.target.matches);
    });

    replaceIcons(!mql.matches);
}

function replaceIcons(usesvg) {
    if ( usesvg ) {
    } else {
        $('svg.icon', $('.tool-menu')).each((i, el) => {
            el = $(el);
            const p = el.parent();
            if ( $('i.icon', p).length == 0 ) {
                const icon_pre_class = el.data("precls");
                const icon_class = el.data("iconname");
                const t = `<i class="icon ${icon_pre_class? icon_pre_class : ''} ${icon_class}" />`;
                $(t).insertAfter(el);
            }
        });
    }
}

function onNewFileClick(e) {
    if ($(e.currentTarget).parent().hasClass('disabled'))
        return;
    
    var me = this;
    if (me.click_lock===true) return;
    me.click_lock = true;

    var t;
    switch (e.currentTarget.attributes['action'].value) {
    case 'new:docx': t = 'word'; break;
    case 'new:xlsx': t = 'cell'; break;
    case 'new:pptx': t = 'slide'; break;
    // case 'new:form': t = 'form'; break;
    case 'new:form': t = 'pdfe'; break;
    default: break;
    }

    // if ( !!t ) window.sdk.command("create:new", t);
    window.app.controller.recent.filterRecents(t);

    setTimeout(function(){
        me.click_lock = false;
    }, utils.defines.DBLCLICK_LOCK_TIMEOUT);
}

window.sdk.on('on_native_message', function(cmd, param) {
    let _re_res;
    if (cmd == 'portal:logout') {
        // var short_name = utils.skipUrlProtocol(param);
        // var model = portalCollection.find('name', short_name);
        // !!model && model.set('logged', false);
    } else
    if ( (_re_res = /^panel\:(hide|show|select)/.exec(cmd)) ) {
        let panel = param;
        if ( _re_res[1] == 'select' ) {
            selectAction( panel );
        } else {
            let hide = !(_re_res[1] == 'show');

            if ( panel.length ) {
                hideAction(panel, hide);
            }
        }
    } else
    if (/app\:ready/.test(cmd)) {
        setLoaderVisible(false);
    }
    
    console.log(cmd, param);
});

window.last_click_time = performance.now();
function openFile(from, model) {
    if (window.sdk) {
        if ( performance.now() - last_click_time < 1000 ) return;
        last_click_time = performance.now();

        if (from == OPEN_FILE_FOLDER) {
            window.sdk.command("open:folder", model);
        } else {
            const params = {
                    id: model.fileid,
                    name: utils.fn.decodeHtml(model.name),
                    path: utils.fn.decodeHtml(model.path),
                    type: model.type,
                    cloud: model.cloud,
                    recovery: from == OPEN_FILE_RECOVERY,
                };

            // if ( from == OPEN_FILE_RECOVERY ) {
                // window.sdk.LocalFileOpenRecover(parseInt(params.id));       // for bug 60509. change on "open:recovery" event for ver 7.4
                // window.sdk.command("open:recovery", JSON.stringify(params));
            // } else {
                // window.sdk.LocalFileOpenRecent(parseInt(params.id));        // for bug 60509. change on "open:recent" event for ver 7.4
                window.sdk.command("open:recent", JSON.stringify(params));
            // }
        }
    } 
}

document.getElementById('wrap').ondrop = function(e) {
    window.sdk["DropOfficeFiles"]();

    e.preventDefault();
    return false;
}

document.getElementById('wrap').ondragover = function (e) {
    e.dataTransfer.dropEffect = 'copy';

    e.preventDefault();
    return false;
};

$(document).on('keydown', function(e){
    if ( e.ctrlKey && e.which == 79 ) {
        if ($('.action-panel').filter('.recent, .open, .welcome').is(':visible')) {
            openFile(OPEN_FILE_FOLDER, '');
        }
    }
});

window.addEventListener('message', e => {
    let msg = window.JSON.parse(e.data);
    if ( msg.type == 'plugin' ) {
        if ( !!msg.event ) {
            if ( msg.event == 'modal:open' )
                $('.main-column.tool-menu').addClass('view--modal')
            else $('.main-column.tool-menu').removeClass('view--modal');
        } else sdk.fire('on_native_message', Object.values(msg.data));
    }
}, false);
